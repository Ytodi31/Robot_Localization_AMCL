#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
  ball_chaser::DriveToTarget srv;

  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;

  if(!client.call(srv))
    ROS_ERROR("Failed to call the service to chase the ball");

}

void process_image_callback(const sensor_msgs::Image img)
{
  int white_pixel = 255;
  auto height = img.height;
  auto width = img.step;
  float ball_pos = 0;
  float lin_x = 0;
  float ang_z = 0;
  for (auto i =0; i< height*width; i=i+3){
      if(img.data[i]==255 && img.data[i+1]==255 && img.data[i+2]==255){
        ball_pos = i%width;
        break;
    }
  }
  if(ball_pos>0 && ball_pos<width/3)
  {
    ROS_INFO_STREAM("Ball detected, moving left");
    lin_x = 0.5;
    ang_z = 20;
  }
  else if(ball_pos>=width/3 && ball_pos<2*width/3)
  {
    ROS_INFO_STREAM("Ball detected, moving straight");
    lin_x = 0.5;
    ang_z = 0;
  }
  else if (ball_pos>=2*width/3 && ball_pos<width)
  {
    ROS_INFO_STREAM("Ball detected, moving right");
    lin_x = 0.5;
    ang_z = -20;
  }
  drive_robot(lin_x, ang_z);
}

int main(int argc, char**argv)
{
  ros::init(argc, argv, "process_image");

  ros::NodeHandle n;

  client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

  ros::Subscriber sub = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

  ros::spin();

  return 0;
}
