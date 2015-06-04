#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "zmq.hpp"
#include "zmq.h"
#include "MultiData.h"
#include "Util.h"

using namespace std; 
static const std::string OPENCV_WINDOW = "Image window";

class ImageConverter
{
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/image_raw", 1, 
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

    sender = new zmq::socket_t(context, ZMQ_PUSH);    
    try
      {
	sender->connect( "tcp://133.19.23.224:8081" );
      }
    catch ( zmq::error_t e )
      {
	std::cout << "bind error" << ( std::string ) e.what( ) << std::endl;
      } 
    
    senderName = "WebcameraServer";
    imagepack[ "senderName" ] = MultiData( senderName, "string", "" );

    width = 640;
    height = 480;

    px = 0;
    py = 0;

    index = 0;

    /*
    responder = new zmq::socket_t(context, ZMQ_REQ);
    try
      {
	responder->connect("tcp://133.19.23.33:50001");
      }
    catch ( zmq::error_t e )
      {
	std::cout << "bind error" << ( std::string ) e.what( ) << std::endl;
      }
    */ 
    cv::namedWindow(OPENCV_WINDOW);
  }

  ~ImageConverter()
  {
    cv::destroyWindow(OPENCV_WINDOW);
  }



  void imageCb(const sensor_msgs::ImageConstPtr& msg)
  {
    cv_bridge::CvImagePtr cv_ptr;
    try
    {
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }

    cv::Mat src_img;
    src_img = cv_ptr->image;

    px = (double)1/2*( src_img.cols - width )*( sin(index*M_PI/180) + 1 );
    py = (double)1/2*( src_img.rows - height );

    cout <<"sin:"<<sin(index*M_PI/180)<<endl;
    cout <<"width:" << src_img.cols<<", height:" << src_img.rows << endl;
    cout <<"px:" <<px << ", py:"<< py <<", index:" << index <<endl;
    ++index;

    cv::Mat cut_img(src_img, cv::Rect(px, py, width, height));
    //cv::Mat img = cut_img;
    cv::Mat rsz_img;
    //cv::resize(cut_img, rsz_img, cv::Size(), 0.5, 1);
    cv::resize(cut_img, rsz_img, cv::Size(), 0.5, 1);
    cv::Mat base_img(cv::Size(640,480),CV_8UC3);
    cv::Mat Roi(base_img, cv::Rect(160,0,rsz_img.cols, rsz_img.rows));
    rsz_img.copyTo(Roi);
    cv::Mat img = base_img;


    imagepack[ "image" ] = MultiData( Util::convertMat2String(img), "image", "input" );
    
    
    msgpack::sbuffer sbuf;
    msgpack::pack( sbuf, imagepack );
    zmq::message_t message( sbuf.size() );
    memcpy( message.data( ), sbuf.data( ), sbuf.size( ) );
    sender->send( message );
    
    /*
    // Draw an example circle on the video stream
    if (cv_ptr->image.rows > 60 && cv_ptr->image.cols > 60)
      cv::circle(cv_ptr->image, cv::Point(50, 50), 10, CV_RGB(255,0,0));

    // Update GUI Window
    */
    cv::imshow(OPENCV_WINDOW, img);
    cv::waitKey(1);
    

    // Output modified video stream
    //image_pub_.publish(cv_ptr->toImageMsg());
  }


private:
  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::Subscriber image_sub_;
  image_transport::Publisher image_pub_;
  
  zmq::context_t context;
  zmq::socket_t *sender; 
  std::map<std::string, MultiData> imagepack;
  std::string senderName;

  //zmq::context_t context_okao;
  //zmq::socket_t* responder; 

  int width, height;
  int px, py;

  int index;
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "web_okao_client");
  ImageConverter ic;
  ros::spin();
    
  return 0;
}
