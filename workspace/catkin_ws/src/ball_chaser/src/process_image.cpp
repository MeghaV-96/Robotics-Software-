#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
std::vector<double> motor_position{ 0, 0 };
bool moving_state = false;
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
	ball_chaser::DriveToTarget srv;
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

  // Call the command_robot service and pass the requested motor commands
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot service");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    //Find the number of rows and columns in the image matrix
    int row = img.height;
    int column = img.width;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    
    //initialize 3 counters which tell us which section of the image has the largest white area
    //The three sections of the image are left, forward and right.
    // Array ball_location is used to set the location 
    //ball_location[0] -> left
    //ball_location[1] -> forward
    //ball_location[2] -> right
    
    int ball_location[3]={0,0,0};
    size_t n = sizeof(ball_location)/sizeof(ball_location[0]);

        
    for(int i = 0;i<(row*column);i += 3)
    {
	    if(img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
	    {
		    
		    ROS_INFO("White pixel detected");
            
            // array print loop
            for (size_t arr = 0; arr < n; arr++) {
                ROS_INFO_STREAM("ball location = " << ball_location[arr] << " ");
            }
		    
		    if((i%img.width) == (img.width/3))
		    {
			    ball_location[0]++;
		    }
		    else if((i%img.width)>(img.width/3) && (i%img.width)<((img.width*2)/3))
		    {
			    ball_location[1]++;
		    }
		    else if((i%img.width)>((img.width*2/3)))
		    {
			    ball_location[2]++;
		    }
	    }
    }

    

    if(ball_location[0] == 0 && ball_location[1] == 0 && ball_location[2] == 0 )
    {
	    drive_robot(0.0,0.0);
    }
    else if(ball_location[0] > ball_location[1] && ball_location[0] > ball_location[2])
    {
	    drive_robot(0.5,0.5);
    }
    else if(ball_location[1]>ball_location[0] && ball_location[1]>ball_location[2])
    {
	    drive_robot(0.5,0.0);
    }
    else if(ball_location[2] > ball_location[0] && ball_location[2]>ball_location[1])
    {
	    drive_robot(0.5,-0.5);
    }
    
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");

    ROS_INFO("process image ready");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
