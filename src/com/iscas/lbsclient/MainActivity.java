package com.iscas.lbsclient;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.LayoutInflater; 
import android.view.View;
import android.app.AlertDialog; 
import android.app.Dialog;   
import android.content.DialogInterface;   
import android.widget.EditText;
import android.widget.TextView;
import java.util.*;
import android.os.Handler;
import android.os.Message;
import org.apache.http.HttpResponse;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.util.EntityUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.baidu.mapapi.*;
import com.baidu.mapapi.map.*;
import com.baidu.mapapi.model.*;

public class MainActivity extends ActionBarActivity {
	private MapView mMapView = null;
	private int mPort = 10001;
	private String mHost = "http://123.57.237.171";
	private ArrayList<Marker> mMarkerList = new ArrayList<Marker>();
	private LatLng mLatLng = null;
	private Handler mHandler = null;
	private int mRt = 0;
	private String mExecuteType = "";
	private static final int FAIL_MESSAGE = 0x01;
	private static final int SUCCESS_MESSAGE = 0x02;
	
	// Http���������첽Handler
	private class AsynHttpHandler extends Handler {		
		public void dispatchMessage(Message msg) {
			handleHttpResponse(msg);
		}
	}
	
	// http����
	private class HttpGetRunnable implements Runnable {
		String mUrl = null;
		
		public HttpGetRunnable(String url) {
			mUrl = url;
		}
		public void run() {
			HttpGet httpGet = new HttpGet(mUrl);
			try {
				HttpResponse httpResponse = HttpClientHelper.getHttpClient().execute(httpGet);;			
				byte[] bytes = EntityUtils.toByteArray(httpResponse.getEntity());
				String result = new String(bytes);	
				Message msg = mHandler.obtainMessage();
				msg.obj  = result;
				msg.what = SUCCESS_MESSAGE;
				mHandler.sendMessage(msg);
			} catch (Exception ex) {
				ex.printStackTrace();
				mHandler.sendEmptyMessage(FAIL_MESSAGE);
			}
		}
	}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);   
        //��ʹ��SDK�����֮ǰ��ʼ��context��Ϣ������ApplicationContext  
        //ע��÷���Ҫ��setContentView����֮ǰʵ��  
        SDKInitializer.initialize(getApplicationContext());  
        setContentView(R.layout.activity_main); 
        //��ȡ��ͼ�ؼ�����  
        mMapView = (MapView) findViewById(R.id.bmapView);
        mHandler = new AsynHttpHandler();
        //���õ�ͼ����
        mMapView.getMap().setMaxAndMinZoomLevel(21, 15);
    }
    
    @Override  
    protected void onDestroy() {  
        super.onDestroy();  
        //��activityִ��onDestroyʱִ��mMapView.onDestroy()��ʵ�ֵ�ͼ�������ڹ���  
        mMapView.onDestroy();  
    }
    
    @Override  
    protected void onResume() {  
        super.onResume();  
        //��activityִ��onResumeʱִ��mMapView. onResume ()��ʵ�ֵ�ͼ�������ڹ���  
        mMapView.onResume();  
    }
    
    @Override  
    protected void onPause() {  
        super.onPause();  
        //��activityִ��onPauseʱִ��mMapView. onPause ()��ʵ�ֵ�ͼ�������ڹ���  
        mMapView.onPause();  
    } 


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        switch (id) {
        case R.id.action_settings:
        	return triggerSettingDialog();
        case R.id.action_range_query:
        	mRt = 0;
        	mExecuteType = "��Χ��ѯ";
        	return rangeQuery();
        case R.id.action_mark:
        	return markPoint();
        case R.id.action_nn_query:
        	mRt = 0;
        	mExecuteType = "���ڲ�ѯ";
        	return nnQuery();
        case R.id.action_status:
        	String info = this.mExecuteType + " ��ʱ�� " + mRt + "(us)";
        	this.showDebugMessage(info);
        	break;
        }
        return super.onOptionsItemSelected(item);
    }
    
    /**
     * �������öԻ���
     */
    private boolean triggerSettingDialog() {
    	final EditText et = new EditText(this); 
    	new AlertDialog.Builder(this).
     			setTitle("���ö˿ں�").
     			setView(et).setPositiveButton("ȷ��", new DialogInterface.OnClickListener() {
 					@Override
 					public void onClick(DialogInterface arg0, int arg1) {
 						// TODO Auto-generated method stub
 						try {
 							mPort = Integer.valueOf(et.getText().toString()).intValue();
 						} catch (Exception e) { }
 					}
 				}).setNegativeButton("ȡ��", null).show();
    	return true;
    }
    
    /**
     * �������˵���Ӧ
     */
    public void handleHttpResponse(Message msg) {
    	switch (msg.what) {
    	case FAIL_MESSAGE:
    		this.showDebugMessage(mHost + ":" + mPort + " ����ʧ��");
    		break;
    	case SUCCESS_MESSAGE:
    		parseAndMarkPoint((String)msg.obj);
    		break;
    	}
    }
    
    /**
     * ����json�����mark����
     * @param result
     */
    private void parseAndMarkPoint(String result) {
    	try {
    		JSONObject json_root = new JSONObject(result);
    		JSONArray mov_list = json_root.getJSONArray("list");
    		for (int i = 0; i < mov_list.length(); ++i) {
    			JSONObject mov = mov_list.getJSONObject(i);
    			double lon = mov.getDouble("lon");
    			double lat = mov.getDouble("lat");
    			LatLng point = new LatLng(lat, lon);
    			this.markPoint(point, R.drawable.taxi);
    		}
    		mRt = json_root.getInt("rt");
    	} catch (JSONException e) {
    		this.showDebugMessage(e.getMessage());
    	}
    }
    
    /**
     * ��Χ��ѯ
     * @return
     */
    private boolean rangeQuery() {
    	clearMakers();
    	LatLngBounds bound = mapBounds();
    	String url = new String();
    	url = mHost + ":" + String.valueOf(mPort) +
    			"?query=range&range=" +
    			String.valueOf(bound.southwest.longitude) + "," +
    			String.valueOf(bound.northeast.longitude) + "," +
    			String.valueOf(bound.southwest.latitude) + "," +
    			String.valueOf(bound.northeast.latitude);
    	ThreadPoolUtils.execute(new HttpGetRunnable(url));
    	return true;
    }
    
    /**
     * ��ע����ڵı�ע��
     * @return
     */
    private boolean markPoint() {
    	clearMakers();
    	mLatLng = mapCenter();
    	markPoint(mLatLng, R.drawable.position);
    	return true;
    }
    
    /***
     * ����ڲ�ѯ
     * @return
     */
    private boolean nnQuery() {
    	if (mLatLng != null) {
    		String url = mHost + ":" + String.valueOf(mPort) +
    				"?query=nn&pos=" + String.valueOf(mLatLng.longitude) + "," +
    				String.valueOf(mLatLng.latitude);
    		ThreadPoolUtils.execute(new HttpGetRunnable(url));
    	}
    	return true;
    }
    
    /**
     * ��ȡ��ͼ��Bound
     * @return
     */
    private LatLngBounds mapBounds() {
    	return mMapView.getMap().getMapStatus().bound;
    }

    /**
     * ��ȡ��ͼ���ĵ�
     * @return
     */
    private LatLng mapCenter() {
    	return mMapView.getMap().getMapStatus().target;
    }
    
    /**
     * �������Markers
     */
    private void clearMakers() {
    	for (Marker marker : mMarkerList) {
    		marker.remove();
    	}
    	mMarkerList.clear();
    }
    
    /**
     * ��mLatLng�㣬��res��Դ���б�ע
     * @param mLatLng
     * @param res
     */
    private void markPoint(LatLng point, int res) {
    	//����Markerͼ��
    	BitmapDescriptor bitmap = BitmapDescriptorFactory  
    		    .fromResource(res);
    	//����MarkerOption�������ڵ�ͼ�����Marker  
    	OverlayOptions option = new MarkerOptions()  
    	    .position(point)  
    	    .icon(bitmap);
    	//�ڵ�ͼ�����Marker������ʾ 
    	mMarkerList.add((Marker)mMapView.getMap().addOverlay(option));
    }
    
    /**
     * ����debug��Ϣ�ĶԻ���
     * @param msg
     */
    private void showDebugMessage(String msg) {
    	Dialog alertDialog = new AlertDialog.Builder(this).   
                setTitle("debug").   
                setMessage(msg).   
                setIcon(R.drawable.ic_launcher).   
                create();   
        alertDialog.show();
    }
}
