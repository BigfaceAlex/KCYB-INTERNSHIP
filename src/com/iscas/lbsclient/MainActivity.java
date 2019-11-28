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
	
	// Http请求后处理的异步Handler
	private class AsynHttpHandler extends Handler {		
		public void dispatchMessage(Message msg) {
			handleHttpResponse(msg);
		}
	}
	
	// http请求
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
        //在使用SDK各组件之前初始化context信息，传入ApplicationContext  
        //注意该方法要再setContentView方法之前实现  
        SDKInitializer.initialize(getApplicationContext());  
        setContentView(R.layout.activity_main); 
        //获取地图控件引用  
        mMapView = (MapView) findViewById(R.id.bmapView);
        mHandler = new AsynHttpHandler();
        //设置地图缩放
        mMapView.getMap().setMaxAndMinZoomLevel(21, 15);
    }
    
    @Override  
    protected void onDestroy() {  
        super.onDestroy();  
        //在activity执行onDestroy时执行mMapView.onDestroy()，实现地图生命周期管理  
        mMapView.onDestroy();  
    }
    
    @Override  
    protected void onResume() {  
        super.onResume();  
        //在activity执行onResume时执行mMapView. onResume ()，实现地图生命周期管理  
        mMapView.onResume();  
    }
    
    @Override  
    protected void onPause() {  
        super.onPause();  
        //在activity执行onPause时执行mMapView. onPause ()，实现地图生命周期管理  
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
        	mExecuteType = "范围查询";
        	return rangeQuery();
        case R.id.action_mark:
        	return markPoint();
        case R.id.action_nn_query:
        	mRt = 0;
        	mExecuteType = "近邻查询";
        	return nnQuery();
        case R.id.action_status:
        	String info = this.mExecuteType + " 耗时： " + mRt + "(us)";
        	this.showDebugMessage(info);
        	break;
        }
        return super.onOptionsItemSelected(item);
    }
    
    /**
     * 弹出设置对话框
     */
    private boolean triggerSettingDialog() {
    	final EditText et = new EditText(this); 
    	new AlertDialog.Builder(this).
     			setTitle("设置端口号").
     			setView(et).setPositiveButton("确定", new DialogInterface.OnClickListener() {
 					@Override
 					public void onClick(DialogInterface arg0, int arg1) {
 						// TODO Auto-generated method stub
 						try {
 							mPort = Integer.valueOf(et.getText().toString()).intValue();
 						} catch (Exception e) { }
 					}
 				}).setNegativeButton("取消", null).show();
    	return true;
    }
    
    /**
     * 处理服务端的响应
     */
    public void handleHttpResponse(Message msg) {
    	switch (msg.what) {
    	case FAIL_MESSAGE:
    		this.showDebugMessage(mHost + ":" + mPort + " 访问失败");
    		break;
    	case SUCCESS_MESSAGE:
    		parseAndMarkPoint((String)msg.obj);
    		break;
    	}
    }
    
    /**
     * 解析json结果并mark数据
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
     * 范围查询
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
     * 标注最近邻的标注点
     * @return
     */
    private boolean markPoint() {
    	clearMakers();
    	mLatLng = mapCenter();
    	markPoint(mLatLng, R.drawable.position);
    	return true;
    }
    
    /***
     * 最近邻查询
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
     * 获取地图的Bound
     * @return
     */
    private LatLngBounds mapBounds() {
    	return mMapView.getMap().getMapStatus().bound;
    }

    /**
     * 获取地图中心点
     * @return
     */
    private LatLng mapCenter() {
    	return mMapView.getMap().getMapStatus().target;
    }
    
    /**
     * 清除所有Markers
     */
    private void clearMakers() {
    	for (Marker marker : mMarkerList) {
    		marker.remove();
    	}
    	mMarkerList.clear();
    }
    
    /**
     * 对mLatLng点，用res资源进行标注
     * @param mLatLng
     * @param res
     */
    private void markPoint(LatLng point, int res) {
    	//构建Marker图标
    	BitmapDescriptor bitmap = BitmapDescriptorFactory  
    		    .fromResource(res);
    	//构建MarkerOption，用于在地图上添加Marker  
    	OverlayOptions option = new MarkerOptions()  
    	    .position(point)  
    	    .icon(bitmap);
    	//在地图上添加Marker，并显示 
    	mMarkerList.add((Marker)mMapView.getMap().addOverlay(option));
    }
    
    /**
     * 弹出debug信息的对话框
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
