#include "Util.h"
#include "base64.h"
using namespace std;
using namespace cv;
namespace Util
{
  /* FUNCTION	: toString
   * �l�X�Ȍ^����string�^�ɕϊ�����
   *
   * Param	: int			: n		: ������ɕϊ����������l(int�^)
   * Return	: std::string	: �ϊ���̕�����
   */
  std::string toString( int n )
  {
    std::stringstream ss; ss << n; return ss.str( );
  }
  
  /* FUNCTION	: replaceAll
   * �n���ꂽ������ɑ΂���replace(�u��)���\�Ȍ���s��
   * "aaaabaaaababaab"�ɑ΂��Ă��ׂĂ�b��c�ɒu������Ƃ���
   * ���̌���"aaaacaaaacacaac"�ƂȂ�
   *
   * Param	: std::string &			: str	:�����񑀍�������镶����
   *				: const std::string	: from	:�ύX��������������
   *				: const std::string	: to	:�ύX��̕���
   * Return	: void				: �Ȃ�
   */
  void replaceAll( std::string &str, const std::string from, const std::string to )
  {
    std::string::size_type pos = 0;
    while ( ( pos = str.find( from ) ) != std::string::npos )
      {
	str.replace( pos, from.size( ), to );
      }
  }

  /* FUNCTION	: split
   * �n���ꂽ������ɑ΂��Ďw�蕶�����split(����)���\�Ȍ���s��
   * Param	: sstring	: s	:�����񑀍�������镶����(���̕�����)
   * Return	: vector	: �������ꂽ�����񂪓����Ă��铮�I�z��
   */
  std::vector<std::string> split( std::string s, std::string t )
  {
    std::vector<std::string> v;
    for ( int p = 0; ( p = s.find( t ) ) != s.npos; )
      {
	v.push_back( s.substr( 0, p ) );
	s = s.substr( p + t.size( ) );
      }
    v.push_back( s );
    return v;
  }
  
  std::vector<std::string> splitFirst( std::string s, const std::string t )
  {
    std::vector<std::string> v;
    int p = s.find( t );
    if ( p != s.npos )
      {
	v.push_back( s.substr( 0, p ) );
	s = s.substr( p + t.size( ) );
      }
    v.push_back( s );
    return v;
  }
  
  
  cv::Mat convertVec2Mat( const std::vector<unsigned char>& vec )
  {
    return cv::imdecode( cv::Mat( vec ), CV_LOAD_IMAGE_UNCHANGED );
  }
  
  
  cv::Mat convertString2Mat( std::string str )
  {
    vector< uchar > buf( str.begin( ), str.end( ) );
    buf.push_back( '0' );
    
    return cv::imdecode( cv::Mat( buf ), CV_LOAD_IMAGE_UNCHANGED );
  }
  
  std::string convertMat2String( cv::Mat mat )
  {
    if ( mat.data == NULL )
      {
	return "";
      }
    std::vector< unsigned char > buf;
    std::vector< int > param = std::vector< int >( 2 );
    param[ 0 ] = CV_IMWRITE_JPEG_QUALITY;
    param[ 1 ] = 95;
    cv::imencode( ".jpg", mat, buf, param );
    std::istringstream iss( string( buf.begin( ), buf.end( ) ) );
    std::ostringstream oss;
    base64::Encode( iss, oss );
    return oss.str( );
  }

}
