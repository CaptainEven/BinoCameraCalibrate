#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>


using namespace cv;
using namespace std;

void add_obj_pts(const Size& board_size, const int square_size, vector<Point3f>& obj_pts);


bool image_capture = false;//定义全局变量，用于相机采集标定图像
//鼠标事件响应函数，用于相机采集标定图像（双击鼠标左键一次采集一张图像）
void on_mouse(int event, int x, int y, int flags, void* a)
{
	if (event == EVENT_LBUTTONDBLCLK)
	{
		image_capture = true;
	}

}


int main(int argc, char* argv[])
{
	Size board_size = Size(10, 8);   // 标定棋盘格的内角点尺寸(如7x7): cols, rows
	float square_size = 20.0;        // 标定板上黑白格子的实际边长（mm）
	int nFrames = 20;                // 用于标定的图像数目
	string output_file_name;         // 输出文件的名称
	bool show_undistorted = true;    // 是否可视化畸变矫正
	vector<string> img_list_1;       // 左视图图像名称(路径)列表
	vector<string> img_list_2;       // 右视图图像名称(路径)列表
	Size img_size;                   // 输入标定图像的尺寸

	int calib_pattern = 0;
	cout << "这是一个双目视觉程序！" << endl;
	cout << "首先，请选择摄像机标定模式：1（从摄像头采集标定图像）或2（从图像序列获取标定图像）" << endl;
	cin >> calib_pattern;

	/************************************************************************************/
	/*********************摄像机实时采集标定图像并保存到指定文件夹***********************/
	//若输入“1”，则打开相机，并利用相机预览窗口和鼠标操作采集标定图像
	//需要采集图像的数目由变量nrFrames决定，采够到nrFrames张图像，预览窗口自动关闭
	if (calib_pattern == 1)
	{
		//VideoCapture inputCapture;
		VideoCapture input_capture_1;
		VideoCapture input_capture_2;
		input_capture_1.open(1);
		input_capture_2.open(2);

		//if(!inputCapture.isOpened()==true) return -1;
		if (!input_capture_1.isOpened() == true) return -1;
		if (!input_capture_2.isOpened() == true) return -1;

		//inputCapture.set(CV_CAP_PROP_FRAME_WIDTH, 640);  
	   // inputCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 480); 

		input_capture_1.set(CAP_PROP_FRAME_WIDTH, 960);//设置所采集图像的分辨率大小  
		input_capture_1.set(CAP_PROP_FRAME_HEIGHT, 720);
		input_capture_2.set(CAP_PROP_FRAME_WIDTH, 960);
		input_capture_2.set(CAP_PROP_FRAME_HEIGHT, 720);

		/*
		inputCapture1.set(CV_CAP_PROP_FRAME_WIDTH, 640);//设置所采集图像的分辨率大小
		inputCapture1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		inputCapture2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		inputCapture2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		*/

		cv::namedWindow("左相机(相机1)标定图像采集预览窗口", WINDOW_AUTOSIZE);
		cv::namedWindow("右相机(相机2)标定图像采集预览窗口", WINDOW_AUTOSIZE);

		//设置鼠标事件函数，用于相机采集标定图像（在指定窗口双击鼠标左键一次采集一张图像）
		cv::setMouseCallback("左相机（相机1）标定图像采集预览窗口", on_mouse, NULL);

		//Mat src_image;
		Mat src_img_1;
		Mat src_img_2;
		int capture_count = 0;

		while (1)
		{
			input_capture_1 >> src_img_1;
			input_capture_2 >> src_img_2;

			imshow("左相机(相机1)标定图像采集预览窗口", src_img_1);
			imshow("右相机(相机2)标定图像采集预览窗口", src_img_2);
			waitKey(35);

			if (image_capture == true && capture_count < nFrames)
			{
				Mat cap1;
				Mat cap2;

				input_capture_1 >> cap1;
				input_capture_2 >> cap2;
				char address[100];

				// 设置标定图像存放路径, 并保存
				sprintf(address, "Calibration_Image_Camera/Image_l%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap1);
				sprintf(address, "Calibration_Image_Camera/Image_r%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap2);

				capture_count++;
				image_capture = false;
			}
			else if (capture_count >= nFrames)
			{
				cout << "标定图像采集完毕！共采集到" << capture_count << "张标定图像。" << endl;
				destroyWindow("左相机(相机1)标定图像采集预览窗口");
				destroyWindow("右相机(相机2)标定图像采集预览窗口");
				image_capture = false;
				break;
			}
		}
	}

	/***********************************************************************************/
	/*******************将存放标定图像的路径读入到imageList（1/2）向量中****************/
	if (calib_pattern == 1 || calib_pattern == 2)
	{
		char name[100];
		for (int i = 1; i <= nFrames; i++)
		{
			sprintf(name, "Calibration_Image_Camera1/Image_l%d%s", i, ".jpg");
			img_list_1.push_back(name);
			sprintf(name, "Calibration_Image_Camera1/Image_r%d%s", i, ".jpg");
			img_list_2.push_back(name);
		}
	}
	cout << "Image list 1 size:" << img_list_1.size() << endl;
	cout << "Image list 2 size:" << img_list_2.size() << endl;

	/************************************************************************************/
	/****************标定前的准备工作：获取Object Points和Image Points*********************/
	//1 首先根据square size(棋盘格的实际边长，比如20mm)和board size(棋盘格的角点数，比如5x5)，
	// 利用for循环得到角点的世界坐标Object Points（Z坐标假设为0）
	//2 利用for循环和findChessboardCorners()函数得到与角点世界坐标向量objectPoints对应的图像像素坐标向量imagePoints

	vector<vector<Point2f>> img_pts_1;     // 存放左视图所有图像的平面角点 
	vector<vector<Point3f>> obj_pts_1(1);  //暂时先定义一维的obj_pts_1，等确定了img_pts的维数之后再进行扩容
	vector<vector<Point2f>> img_pts_2;     // 存放右视图所有图像的平面角点
	vector<vector<Point3f>> obj_pts_2(1);  // 存放右视图所有空间角点

	// ---------- 处理左视图
	// 获取角点的世界坐标系坐标
	add_obj_pts(board_size, (int)square_size, obj_pts_1[0]);

	//可通过改变变量displayCorners1的值来确定是否展示获取角点后的图像
	bool display_corners_1 = true;

	// 计算左视图所有的平面角点
	for (int i = 0; i < img_list_1.size(); ++i)
	{
		Mat src_1 = imread(img_list_1[i], 1);
		img_size = src_1.size();
		vector<Point2f> pts_buff_1;

		//使用不同的FLAG变量，标定结果差别很小
		bool found_1 = cv::findChessboardCorners(src_1, board_size, pts_buff_1);
		//bool found_1 = findChessboardCorners(src1, board_size, pt_buff_1, CALIB_CB_ADAPTIVE_THRESH);
		//bool found_1 = findChessboardCorners(src1, board_size, pt_buff_1, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);  // 这一步比较费时

		if (found_1)
		{
			Mat gray_img_1;
			cv::cvtColor(src_1, gray_img_1, COLOR_BGR2GRAY);

			cv::cornerSubPix(gray_img_1,
				pts_buff_1,
				Size(11, 11),
				Size(-1, -1),
				TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));

			// 存放当前左视图平面角点
			img_pts_1.push_back(pts_buff_1);

			if (display_corners_1)
			{
				Mat src_show_1 = src_1.clone();
				cv::drawChessboardCorners(src_show_1, board_size, Mat(pts_buff_1), found_1);
				cv::imshow("左相机角点获取情况", src_show_1);
				cv::waitKey(300);
			}

			cv::destroyWindow("左相机角点获取情况");
		}
	}

	// 对object points向量进行扩容
	obj_pts_1.resize(img_pts_1.size(), obj_pts_1[0]);  // 复制(左视图图像个数)份

	// ---------- 处理右视图
	// 获取角点的世界坐标系坐标
	add_obj_pts(board_size, (int)square_size, obj_pts_2[0]);

	bool display_corners_2 = true;
	for (int i = 0; i < img_list_2.size(); i++)
	{
		Mat src_2 = imread(img_list_2[i], 1);
		vector<Point2f> pts_buff_2;

		//使用不同的FLAG变量，标定结果差别很小
		bool found_2 = cv::findChessboardCorners(src_2, board_size, pts_buff_2);
		//bool found_2 = findChessboardCorners(src_2, board_size, pts_buff_2, CALIB_CB_ADAPTIVE_THRESH);
		//bool found_2 = findChessboardCorners(src_2, board_size, pts_buff_2, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);  // 这一步比较费时

		if (found_2)
		{
			Mat gray_img_2;
			cvtColor(src_2, gray_img_2, COLOR_BGR2GRAY);

			cv::cornerSubPix(gray_img_2,
				pts_buff_2,
				Size(11, 11),
				Size(-1, -1),
				TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));

			img_pts_2.push_back(pts_buff_2);
			if (display_corners_2)
			{
				Mat src_show_2 = src_2.clone();
				cv::drawChessboardCorners(src_show_2, board_size, Mat(pts_buff_2), found_2);
				cv::imshow("右相机角点获取情况", src_show_2);
				cv::waitKey(300);
			}

			cv::destroyWindow("右相机角点获取情况");
		}
	}

	// 对右视图的空间角点容器进行扩容
	obj_pts_2.resize(img_pts_2.size(), obj_pts_2[0]);

	/**************************************************************************************/
	/********************************进行单目相机标定******************************************/
	//通过calibrateCamera()函数进行相机标定
	//主要为了得到相机的内参矩阵cameraMatrix、畸变系数矩阵distCoeffs
	//另外可以通过函数返回的重投影误差大小评价相机标定的精度如何
	//这里得到的相机外参矩阵不重要

	//Mat camera_matrix = Mat::eye(3, 3, CV_64F);
	//Mat distCoeffs = Mat::zeros(8, 1, CV_64F);   // 畸变系数的顺序是[k1,k2,p1,p2,k3,(k4,k5,k6)]
	Mat camera_matrix_1 = Mat::eye(3, 3, CV_64F);  // 左视图相机内参矩阵
	Mat dist_coeffs_1 = Mat::zeros(5, 1, CV_64F);  // 左视图畸变系数 
	Mat camera_matrix_2 = Mat::eye(3, 3, CV_64F);  // 右视图相机内参矩阵
	Mat dist_coeffs_2 = Mat::zeros(5, 1, CV_64F);  // 右视图畸变系数

	//vector<Mat> rvecs, tvecs;
	vector<Mat> r_vecs_1, t_vecs_1;  // 左视图每张视图的位姿: 旋转矩阵和平移向量 
	vector<Mat> r_vecs_2, t_vecs_2;  // 右视图每张视图的位姿: 旋转矩阵和平移向量 

	/*
	double re_project_err=calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,0);
	//checkRange()函数---用于检查矩阵中的每一个元素是否在指定的一个数值区间之内
	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
	if(ok)
	{
		cout<<"相机标定成功！"<<endl;
		cout<<"相机标定的重投影误差："<<re_project_err<<endl;
		cout<<"相机内参矩阵："<<endl<<cameraMatrix<<endl;
		cout<<"相机畸变系数矩阵："<<endl<<distCoeffs<<endl;
		if(showUndistorsed==true)
		{
			for(int i=0;i<imageList.size();i++)
			{
				Mat temp=imread(imageList1[i],1);;
				//利用undistort()函数得到经过畸变矫正的图像
				Mat undistort_view;
				undistort(temp, undistort_view, cameraMatrix, distCoeffs);
				imshow("原畸变图像",temp);
				imshow("畸变矫正图像",undistort_view);
				waitKey(300);
			}
			destroyWindow("原畸变图像");
			destroyWindow("畸变矫正图像");
		}
	}
	*/
	/*
	calibrateCamera()
	输入参数 objectPoints  角点的实际物理坐标
			 imagePoints   角点的图像坐标
			 imageSize     图像的大小
	输出参数
			 cameraMatrix  相机的内参矩阵
			 distCoeffs    相机的畸变参数
			 rvecs         旋转矢量(外参数)
			 tvecs         平移矢量(外参数）
	*/

	//畸变系数的顺序是[k1,k2,p1,p2,k3,(k4,k5,k6)]
	//如果我们不需要K3，在初始化K3为O之后，可以使用标志CV_CALIB_FIX_K3，这样，标定函数不会改变K3的值
	//一般地，K3应设置为0，除非使用鱼眼镜头（参考《learning opencv》第十一章）
	//返回的distCoeffs1向量的长度由标志位flag决定，当flag设置为CV_CALIB_RATIONAL_MODEL时返回所有畸变参数（8个）
	//当设置成其他flag时都返回5维的畸变系数，即[k1, k2, p1, p2, k3]

	double re_proj_err_1 = cv::calibrateCamera(obj_pts_1,
		img_pts_1,
		img_size,
		camera_matrix_1,
		dist_coeffs_1,
		r_vecs_1,
		t_vecs_1,
		CALIB_FIX_K3);

	//checkRange()函数  ---用于检查矩阵中的每一个元素的有效性
	bool ok_1 = cv::checkRange(camera_matrix_1) && checkRange(dist_coeffs_1);
	if (ok_1)
	{
		printf("\n");
		cout << "左相机标定成功！" << endl;
		cout << "左相机标定的重投影误差：" << re_proj_err_1 << "pixel" << endl;
		cout << "左相机内参矩阵：" << endl << camera_matrix_1 << endl;
		cout << "左相机畸变系数矩阵：" << endl << dist_coeffs_1 << endl;

		if (show_undistorted == true)
		{
			for (int i = 0; i < img_list_1.size(); i++)
			{
				Mat temp = imread(img_list_1[i], 1);

				//利用undistort()函数得到经过畸变矫正的图像
				Mat undistort_view;

				cv::undistort(temp, undistort_view, camera_matrix_1, dist_coeffs_1);

				imshow("原畸变图像", temp);
				imshow("畸变矫正图像", undistort_view);
				waitKey(300);
			}

			destroyWindow("原畸变图像");
			destroyWindow("畸变矫正图像");
		}
	}

	double re_project_err_2 = calibrateCamera(obj_pts_2,
		img_pts_2,
		img_size,
		camera_matrix_2,
		dist_coeffs_2,
		r_vecs_2,
		t_vecs_2,
		CALIB_FIX_K3);

	bool ok_2 = checkRange(camera_matrix_2) && checkRange(dist_coeffs_2);
	if (ok_2)
	{
		printf("\n");
		cout << "右相机标定成功！" << endl;
		cout << "右相机标定的重投影误差：" << re_project_err_2 << "pixel" << endl;
		cout << "右相机内参矩阵：" << endl << camera_matrix_2 << endl;
		cout << "右相机畸变系数矩阵：" << endl << dist_coeffs_2 << endl;

		if (show_undistorted == true)
		{
			for (int i = 0; i < img_list_2.size(); i++)
			{
				Mat temp = imread(img_list_2[i], 1);

				//利用undistort()函数得到经过畸变矫正的图像
				Mat undistort_view;

				cv::undistort(temp, undistort_view, camera_matrix_2, dist_coeffs_2);

				imshow("原畸变图像", temp);
				imshow("畸变矫正图像", undistort_view);
				waitKey(300);
			}

			destroyWindow("原畸变图像");
			destroyWindow("畸变矫正图像");
		}
	}

	/**************************************************************************************/
	/********************************立体标定(双目标定)******************************************/
	//利用stereoCalibrate()函数进行立体标定，得到4个矩阵：R 旋转矢量 T平移矢量 E本征矩阵 F基础矩阵
	//同时可以得到两个相机的内参矩阵和畸变系数矩阵cameraMatrix1、distCoeffs1、cameraMatrix2、distCoeffs2
	//也就是说其实可以不用事先单独给每个相机进行标定的
	//stereoCalibrate()函数内部应该是调用了calibrateCamera()函数的
	//但是也可以先调用calibrateCamera()函数先对每个相机进行标定，得到内参矩阵和畸变系数的初始值
	//然后立体标定时stereoCalibrate()函数会对内参矩阵和畸变系数进行优化，此时应加上flag：CALIB_USE_INTRINSIC_GUESS
	//如果没有事先调用过calibrateCamera()函数，请不要使用flag:CALIB_USE_INTRINSIC_GUESS，会得到很奇怪的结果
	//如果之前标定过的相机内参矩阵和畸变参数很满意，不想在立体标定时被进一步优化，可使用CV_CALIB_FIX_INTRINSIC
	//根据官方文档建议，stereoCalibrate()函数计算的参数空间的维数很高（一次性得到很多结果）
	//可能会导致某些结果发散到无意义的值，偏离正确结果，如果提前使用了calibrateCamera()函数对每个相机进行过标定
	//则可以选择将CALIB_FIX_INTRINSIC应用到stereoCalibrate()函数中，这样能减少计算的参数
	//防止导致某些结果发散到无意义的值
	//CV_CALIB_FIX_INTRINSIC这个参数是否使用还需后面做进一步权衡

	//R: 旋转矢量 T: 平移矢量 E: 本征矩阵 F: 基础矩阵
	Mat R = Mat::eye(3, 3, CV_64F);
	Mat T = Mat::zeros(3, 1, CV_64F);
	Mat E = Mat::zeros(3, 3, CV_64F);
	Mat F = Mat::eye(3, 3, CV_64F);    

	double rms = cv::stereoCalibrate(obj_pts_1, img_pts_1, img_pts_2,
		camera_matrix_1, dist_coeffs_1,
		camera_matrix_2, dist_coeffs_2,
		img_size, R, T, E, F,
		CALIB_FIX_INTRINSIC,
		TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 100, 1e-5));

	/*double rms = stereoCalibrate(objectPoints1, imagePoints1, imagePoints2,
		cameraMatrix1, distCoeffs1,
		cameraMatrix2, distCoeffs2,
		imageSize, R, T, E, F,
		CALIB_USE_INTRINSIC_GUESS|CV_CALIB_FIX_K3,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 30, 1e-6)); */

	printf("\n");
	cout << "Stereo Calibration done with RMS error = " << rms << endl;
	cout << "左相机内参矩阵：" << endl << camera_matrix_1 << endl;
	cout << "左相机畸变系数矩阵：" << endl << dist_coeffs_1 << endl;
	cout << "右相机内参矩阵：" << endl << camera_matrix_2 << endl;
	cout << "右相机畸变系数矩阵：" << endl << dist_coeffs_2 << endl;
	cout << "R:" << endl << R << endl;
	cout << "T:" << endl << T << endl;

	/**************************************************************************************/
	/********************************保存标定结果******************************************/
	//保存.xml文件时需要注意2个问题：
	//1 需要保存的Mat型变量定义时必须要初始化，否则程序编译会出错；
	//2 保存时变量的标识符命名中不能出现“.”。

	const string calib_f_path = "./Calibration_Result.xml";
	FileStorage fs_calib(calib_f_path, FileStorage::WRITE);

	if (fs_calib.isOpened())
	{
		fs_calib << "width" << img_size.width;
		fs_calib << "height" << img_size.height;
		fs_calib << "board_width" << board_size.width;
		fs_calib << "board_height" << board_size.height;
		fs_calib << "nFrames" << nFrames;
		fs_calib << "cameraMatrix1" << camera_matrix_1;
		fs_calib << "distCoeffs1" << dist_coeffs_1;
		fs_calib << "cameraMatrix2" << camera_matrix_2;
		fs_calib << "distCoeffs2" << dist_coeffs_2;
		fs_calib << "R" << R;
		fs_calib << "T" << T;
		fs_calib << "E" << E;
		fs_calib << "F" << F;
		fs_calib.release();
		cout << "Calibration result has been saved successfully to ./Calibration_Result.xml" << endl;
	}
	else
	{
		cout << "Error: can not save the Calibration result!" << endl;
	}

	/********************************立体矫正********************************/
	Mat R1, R2, P1, P2, Q;
	cv::stereoRectify(
		camera_matrix_1,
		dist_coeffs_1,
		camera_matrix_2,
		dist_coeffs_2,
		img_size,
		R, T,
		R1, R2, P1, P2, Q
	);
	cout << "Q:" << endl << Q << endl;

	const string q_f_path = "./Q.xml";
	FileStorage fs_q(q_f_path, FileStorage::WRITE);
	if (fs_q.isOpened())
	{
		fs_q << "Q" << Q;
		fs_q.release();
	}
	else
	{
		cout << "Error: can not save the Q matrix!" << endl;
	}

	/********************************计算校正查找映射表********************************/
	// 将原图像和校正后图像上的点一一映射。
	Mat remapm_x_1 = Mat(img_size, CV_32FC1);
	Mat remapm_y_1 = Mat(img_size, CV_32FC1);
	Mat remapm_x_2 = Mat(img_size, CV_32FC1);
	Mat remapm_y_2 = Mat(img_size, CV_32FC1);

	cv::initUndistortRectifyMap(camera_matrix_1, dist_coeffs_1, R1, P1, img_size, CV_16SC2, remapm_x_1, remapm_y_1);
	cv::initUndistortRectifyMap(camera_matrix_2, dist_coeffs_2, R2, P2, img_size, CV_16SC2, remapm_x_2, remapm_y_2);

	Mat img_1 = imread(img_list_1[0], IMREAD_COLOR);
	Mat img_2 = imread(img_list_2[0], IMREAD_COLOR);
	Mat img_l, img_r;  // 矫正后的左，右视图
	if (!remapm_x_1.empty() && !remapm_y_1.empty())  // ⑤进行矫正，映射
	{
		remap(img_1, img_l, remapm_x_1, remapm_y_1, INTER_LINEAR);  // img_1 -> img_l
	}
	if (!remapm_x_2.empty() && !remapm_y_2.empty())
	{
		remap(img_2, img_r, remapm_x_2, remapm_y_2, INTER_LINEAR);  // img_2 -> img_r
	}
	imshow("imgLr", img_l);
	imshow("imgRr", img_r);

	imwrite("./imgLeft.png", img_l);
	imwrite("./imgRight.png", img_r);  // 保存图片

	/********************************显示矫正效果******************************************/
	// 创建IMG，高度一样，宽度双倍
	Mat img_origin(img_size.height*0.5, img_size.width, CV_8UC3);   // 矫正前的左-右视图
	Mat img_rectify(img_size.height*0.5, img_size.width, CV_8UC3);  // 矫正后的左-右视图

	// 浅拷贝
	Mat img_rectify_part_1 = img_rectify(Rect(0, 0, img_size.width*0.5, img_size.height*0.5));  
	Mat img_rectify_part_2 = img_rectify(Rect(img_size.width*0.5, 0, img_size.width*0.5, img_size.height*0.5));

	// 填充
	resize(img_l, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
	resize(img_r, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

	// 左-右视图画横线
	for (int i = 0; i < img_rectify.rows; i += 16)  
	{
		line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
	}

	// 浅拷贝
	Mat img_part_1 = img_origin(Rect(0, 0, img_size.width*0.5, img_size.height*0.5));
	Mat img_part_2 = img_origin(Rect(img_size.width*0.5, 0, img_size.width*0.5, img_size.height*0.5));

	// 填充
	resize(img_1, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
	resize(img_2, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

	// 左-右视图画横线
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
	}

	// 可视化
	imshow("un-rectified", img_origin);
	imshow("rectified", img_rectify);

	// 输出可视化结果
	imwrite("un-retified.png", img_origin);
	imwrite("retified.png", img_rectify);

	cout << "按任意键退出程序..." << endl;

	//while (1)
	//{
	//	int a = waitKey(10);
	//	if (char(a) == 27)
	//	{
	//		break;
	//	}
	//}

	waitKey(0);
	return 0;
}

/*计算标定板上模块的实际物理坐标*/
void add_obj_pts(const Size& board_size, const int square_size, vector<Point3f>& obj_pts)
{// (points_per_row, points_per_col): (cols, rows)
	for (int y = 0; y < board_size.height; ++y)
	{
		for (int x = 0; x < board_size.width; ++x)
		{
			obj_pts.push_back(Point3f((float)(x*square_size), (float)(y*square_size), 0.0f));
		}
	}
}
