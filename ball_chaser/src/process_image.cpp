#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
  ball_chaser::DriveToTarget drive;
  drive.request.linear_x = lin_x;
  drive.request.angular_z = ang_z;

  ROS_INFO("Robot are moving - linear_x:%1.2f, angular_z:%1.2f", lin_x, ang_z);

  if (!client.call(drive))
      ROS_ERROR("Failed to use drive_bot command");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int pixel_position = -1;

    for (int i = 0; i < img.height * img.step; i++) {
      if (img.data[i] == white_pixel) {
        pixel_position = i % img.step;
        break;
      }
    }

    int left_section  = static_cast<int>(img.step / 3);
    int right_section  = static_cast<int>(2 * img.step / 3);

    // Stop and wait
    if (pixel_position < 0)
      drive_robot(0.0, 0.0);

    // Go left
    if (pixel_position >= 0 && pixel_position < left_section)
      drive_robot(0.1, 0.3);

    // Go Forward
    if (pixel_position > left_section && pixel_position <= right_section)
      drive_robot(0.1, 0.0);

    // Go Right
    if (pixel_position > right_section)
      drive_robot(0.1, -0.3);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
