#include <iostream>
#include <opencv2\opencv.hpp>
#include <zmq.hpp>
#include "MultiData.h"
#include <map>
#include "Util.h"
void show( cv::Mat img )
{
	cv::imshow( "img", img );
	cv::waitKey( 1 );
}
void run( )
{
	cv::VideoCapture cap( 0 );
	if ( !cap.isOpened( ) )
	{
		std::cout << "camera is not opened!!\n";
		return;
	}

	zmq::context_t context( 1 );
	zmq::socket_t sender( context, ZMQ_PUSH );
	try
	{
		sender.connect( "tcp://133.19.23.224:8081" );
	}
	catch ( zmq::error_t e )
	{
		std::cout << "bind error" << ( std::string ) e.what( ) << std::endl;
	}


	const std::string senderName = "WebcameraServer";
	std::map<std::string, MultiData> imagepack;

	imagepack[ "senderName" ] = MultiData( senderName, "string", "" );

	while ( 1 )
	{
		cv::Mat img;
		cap >> img;
		imagepack[ "image" ] = MultiData( Util::convertMat2String(img), "image", "input" );
		imagepack[ "image2" ] = MultiData( Util::convertMat2String( img ), "image", "input" );

		show( img );
		msgpack::sbuffer sbuf;
		msgpack::pack( sbuf, imagepack );
		zmq::message_t message( sbuf.size( ) );
		memcpy( message.data( ), sbuf.data( ), sbuf.size( ) );
		sender.send( message );
	}
}
int main( )
{
	run( );
	return 0;
}