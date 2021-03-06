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


static const std::string OPENCV_WINDOW = "Image window";

class ImageConverter
{
public:
  ImageConverter()
    : it_(nh_)
  {
    // Subscrive to input video feed and publish output video feed
    image_sub_ = it_.subscribe("/usb_cam/image_raw", 1, 
      &ImageConverter::imageCb, this);
    image_pub_ = it_.advertise("/image_converter/output_video", 1);

//context = new zmq::context_t( 1 );
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

    //cv::namedWindow(OPENCV_WINDOW);
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

    cv::Mat img;
    img = cv_ptr->image;
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
    cv::imshow(OPENCV_WINDOW, cv_ptr->image);
    cv::waitKey(3);
    */

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

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_converter");
  ImageConverter ic;
  ros::spin();
  return 0;
}
