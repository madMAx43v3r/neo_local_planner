
#ifndef INCLUDE_NEOLOCALPLANNER_H_
#define INCLUDE_NEOLOCALPLANNER_H_

#include <tf/transform_listener.h>
#include <dynamic_reconfigure/server.h>
#include <angles/angles.h>
#include <nav_msgs/Odometry.h>
#include <nav_msgs/Path.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <nav_core/base_local_planner.h>
#include <base_local_planner/local_planner_util.h>
#include <base_local_planner/local_planner_limits.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


namespace neo_local_planner {

class NeoLocalPlanner : public nav_core::BaseLocalPlanner {
public:
	NeoLocalPlanner();

	~NeoLocalPlanner();

	bool computeVelocityCommands(geometry_msgs::Twist& cmd_vel) override;

	bool isGoalReached() override;

	bool setPlan(const std::vector<geometry_msgs::PoseStamped>& plan) override;

	void initialize(std::string name, tf::TransformListener* tf, costmap_2d::Costmap2DROS* costmap_ros) override;

private:
	void odomCallback(const nav_msgs::Odometry::ConstPtr& msg);

private:
	tf::TransformListener* m_tf = 0;
	costmap_2d::Costmap2DROS* m_cost_map = 0;
	std::vector<geometry_msgs::PoseStamped> m_global_plan;

	boost::mutex m_odometry_mutex;
	nav_msgs::Odometry::ConstPtr m_odometry;

	ros::Subscriber m_odom_sub;
	ros::Publisher m_local_plan_pub;

	std::string m_global_frame = "map";
	std::string m_local_frame = "odom";
	std::string m_base_frame = "base_link";

	base_local_planner::LocalPlannerLimits m_limits = {};

	double m_goal_tune_time = 0;		// [s]
	double m_lookahead_time = 0;		// [s]
	double m_lookahead_dist = 0;		// [m]
	double m_start_yaw_error = 0;		// [rad]
	double m_pos_x_gain = 0;			// [1/s]
	double m_pos_y_gain = 0;			// [1/s]
	double m_pos_y_yaw_gain = 0;		// [rad/s^2]
	double m_yaw_gain = 0;				// [1/s]
	double m_static_yaw_gain = 0;		// [1/s]
	double m_cost_x_gain = 0;
	double m_cost_y_gain = 0;
	double m_cost_y_yaw_gain = 0;
	double m_cost_y_lookahead_dist = 0;	// [m]
	double m_cost_y_lookahead_time = 0;	// [s]
	double m_cost_yaw_gain = 0;
	double m_low_pass_gain = 0;
	double m_max_curve_vel = 0;			// [rad/s]
	double m_max_goal_dist = 0;			// [m]
	double m_max_backup_dist = 0;		// [m]
	double m_max_cost = 0;				// [1]
	double m_min_stop_dist = 0;			// [m]
	double m_emergency_acc_lim_x = 0;	// [m/s^2]

	bool m_differential_drive = false;

	enum state_t {
		STATE_IDLE,
		STATE_TRANSLATING,
		STATE_ROTATING,
		STATE_ADJUSTING,
		STATE_TURNING,
		STATE_STUCK
	};

	state_t m_state = state_t::STATE_IDLE;

	ros::WallTime m_last_time;
	ros::WallTime m_first_goal_reached_time;

	bool m_is_goal_reached = false;
	double m_last_control_values[3] = {};
	geometry_msgs::Twist m_last_cmd_vel;

};


} // neo_local_planner

#endif /* INCLUDE_NEOLOCALPLANNER_H_ */
