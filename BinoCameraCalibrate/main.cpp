#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif


#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "utils.h"
#include "ReadFromXmlAndRectify.h"

#define SHOW false

using namespace cv;
using namespace std;


void add_obj_pts(const Size& board_size, const int square_size, vector<Point3f>& obj_pts);

// 定义全局变量
bool image_capture = false;  // 定义全局变量，用于相机采集标定图像
bool show_image = false;      // 是否显示中间标注过程
char win_name_1[100];        // 窗口1名称
char win_name_2[100];        // 窗口2名称


//鼠标事件响应函数，用于相机采集标定图像（双击鼠标左键一次采集一张图像）
void on_mouse(int event, int x, int y, int flags, void* a)
{
	if (event == EVENT_LBUTTONDBLCLK)
	{
		image_capture = true;
	}
}

int runCalibrateAndRectify();
int readFromXmlAndRectify();

const int getDirs(const string& path, vector<string>& dirs);
const int getFilesFormat(const string& path, const string& format, vector<string>& files);


int main(int argc, char* argv[])
{
	//runCalibrateAndRectify();

	readFromXmlAndRectify();
}


int runCalibrateAndRectify()
{
	Size board_size = Size(10, 8);   // 标定棋盘格的内角点尺寸(如7x7): cols, rows
	const float square_size = 20.0;  // 标定板上黑白格子的实际边长（mm）
	int nFrames = 20;                // 用于标定的图像数目
	string output_file_name;         // 输出文件的名称
	bool show_undistorted = true;    // 是否可视化畸变矫正
	vector<string> img_list_1;       // 左视图图像名称(路径)列表
	vector<string> img_list_2;       // 右视图图像名称(路径)列表
	Size img_size;                   // 输入标定图像的尺寸

	int calib_pattern = 0;  // 初始化
	cout << "这是一个双目视觉标定矫正程序！" << endl;
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
		if (!input_capture_1.isOpened() == true)
		{
			printf("Open camera 1 failed.\n");
			return -1;
		}
		if (!input_capture_2.isOpened() == true)
		{
			printf("Open camera 2 failed.\n");
			return -1;
		}

		// 设置所采集图像的分辨率大小  
		input_capture_1.set(CAP_PROP_FRAME_WIDTH, 960);    // 640
		input_capture_1.set(CAP_PROP_FRAME_HEIGHT, 720);   // 480
		input_capture_2.set(CAP_PROP_FRAME_WIDTH, 960);
		input_capture_2.set(CAP_PROP_FRAME_HEIGHT, 720);

		if (show_image)
		{
			cv::namedWindow("左相机(相机1)标定图像采集预览窗口", WINDOW_AUTOSIZE);
			cv::namedWindow("右相机(相机2)标定图像采集预览窗口", WINDOW_AUTOSIZE);

			//设置鼠标事件函数，用于相机采集标定图像（在指定窗口双击鼠标左键一次采集一张图像）
			cv::setMouseCallback("左相机（相机1）标定图像采集预览窗口", on_mouse, NULL);
		}

		//Mat src_image;
		Mat src_img_1;
		Mat src_img_2;
		int capture_count = 0;

		// 定时采集: 10s间隔
		clock_t start_time;
		clock_t end_time;
		double  duration;
		start_time = clock();

		while (1)
		{
			input_capture_1 >> src_img_1;
			input_capture_2 >> src_img_2;

			if (show_image)  // 如果可以显示窗口
			{
				imshow("左相机(相机1)标定图像采集预览窗口", src_img_1);
				imshow("右相机(相机2)标定图像采集预览窗口", src_img_2);
				waitKey(50);  // 35
			}
			else  // 如果不能显示窗口, 每隔10秒采集一次
			{
				end_time = clock();
				duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
				if (duration > 10)  // 间隔时间10s
				{
					image_capture = true;
				}
			}

			if (image_capture == true && capture_count < nFrames)
			{
				Mat cap1;
				Mat cap2;

				input_capture_1 >> cap1;
				input_capture_2 >> cap2;

				// 设置标定图像存放路径, 并保存
				char address[100];
				sprintf(address, "./Calibration_Image_Camera/Image_l%d%s", capture_count + 1, ".jpg");
				cv::imwrite(address, cap1);

				sprintf(address, "./Calibration_Image_Camera/Image_r%d%s", capture_count + 1, ".jpg");
				cv::imwrite(address, cap2);

				capture_count++;
				image_capture = false;
			}
			else if (capture_count >= nFrames)
			{
				cout << "标定图像采集完毕！共采集到" << capture_count << "张标定图像。" << endl;
				if (show_image)
				{
					cv::destroyWindow("左相机(相机1)标定图像采集预览窗口");
					cv::destroyWindow("右相机(相机2)标定图像采集预览窗口");
				}

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
			sprintf(name, "./Calibration_Image_Camera1/Image_l%d%s", i, ".jpg");
			img_list_1.push_back(name);
			printf("%s loaded.\n", name);

			sprintf(name, "./Calibration_Image_Camera1/Image_r%d%s", i, ".jpg");
			img_list_2.push_back(name);
			printf("%s loaded.\n", name);
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
	printf("\nProcessing the right frames...\n");

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

			if (show_image & display_corners_1)
			{
				sprintf(win_name_1, "左相机角点获取情况");
				Mat src_show_1 = src_1.clone();
				cv::drawChessboardCorners(src_show_1, board_size, Mat(pts_buff_1), found_1);
				cv::imshow(win_name_1, src_show_1);
				cv::waitKey(500);
				cv::destroyWindow(win_name_1);
			}
		}

		printf("[Left frame %d] finding corners done.\n", i + 1);
	}

	// 对object points向量进行扩容
	obj_pts_1.resize(img_pts_1.size(), obj_pts_1[0]);  // 复制(左视图图像个数)份

	// ---------- 处理右视图
	printf("\nProcessing the right frames...\n");

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

			if (show_image & display_corners_2)
			{
				sprintf(win_name_1, "右相机角点获取情况");

				Mat src_show_2 = src_2.clone();
				cv::drawChessboardCorners(src_show_2, board_size, Mat(pts_buff_2), found_2);
				cv::imshow(win_name_1, src_show_2);
				cv::waitKey(500);
				cv::destroyWindow(win_name_1);
			}
		}

		printf("[Right frame %d] finding corners done.\n", i + 1);
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
				waitKey(500);
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

	printf("\nStart calibrate the left camera...\n");
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

				if (show_image)
				{
					sprintf(win_name_1, "原畸变图像");
					sprintf(win_name_2, "畸变矫正图像");
					cv::imshow(win_name_1, temp);
					cv::imshow(win_name_2, undistort_view);
					cv::waitKey(500);
					cv::destroyWindow(win_name_1);
					cv::destroyWindow(win_name_2);
				}
			}
		}
	}
	printf("Calibrate the left camera done.\n");

	printf("\nStart calibrate the right camera...\n");
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

				if (show_image)
				{
					sprintf(win_name_1, "原畸变图像");
					sprintf(win_name_2, "畸变矫正图像");

					cv::imshow(win_name_1, temp);
					cv::imshow(win_name_2, undistort_view);
					cv::waitKey(500);
					cv::destroyWindow(win_name_1);
					cv::destroyWindow(win_name_2);
				}
			}
		}
	}
	printf("Calibrate the right camera done.\n");


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

	printf("\nStart stereo calibrating...\n");

	//R: 旋转矢量 T: 平移矢量 E: 本征矩阵 F: 基础矩阵
	Mat R = Mat::eye(3, 3, CV_64F);
	Mat T = Mat::zeros(3, 1, CV_64F);
	Mat E = Mat::zeros(3, 3, CV_64F);
	Mat F = Mat::eye(3, 3, CV_64F);

	double rms = cv::stereoCalibrate(obj_pts_1, img_pts_1, img_pts_2,  // input
		camera_matrix_1, dist_coeffs_1,                                // input
		camera_matrix_2, dist_coeffs_2,                                // input
		img_size,                                                      // input
		R, T, E, F,                                                    // output
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
	printf("Stereo calibrating done.\n");

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
	printf("Start stereo rectifying...\n");

	Mat R1, R2, P1, P2, Q;
	cv::stereoRectify(
		camera_matrix_1,       // input
		dist_coeffs_1,         // input
		camera_matrix_2,       // input
		dist_coeffs_2,         // input
		img_size,              // input
		R, T,                  // input
		R1, R2, P1, P2, Q      // output
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

	// 读取原始图
	Mat img_1 = cv::imread(img_list_1[0], IMREAD_COLOR);
	Mat img_2 = cv::imread(img_list_2[0], IMREAD_COLOR);

	// 计算矫正后的左，右视图
	Mat left_img_rect, right_img_rect;  
	if (!remapm_x_1.empty() && !remapm_y_1.empty())  // ⑤进行矫正，映射
	{
		cv::remap(img_1, left_img_rect, remapm_x_1, remapm_y_1, INTER_LINEAR);  // img_1 -> img_l
	}
	if (!remapm_x_2.empty() && !remapm_y_2.empty())
	{
		cv::remap(img_2, right_img_rect, remapm_x_2, remapm_y_2, INTER_LINEAR);  // img_2 -> img_r
	}
	if (show_image)  // 可视化
	{	
		cv::imshow("imgLr", left_img_rect);
		cv::imshow("imgRr", right_img_rect);
	}
	printf("Stereo rectifying done.\n");

	cv::imwrite("./imgLeft.png", left_img_rect);
	cv::imwrite("./imgRight.png", right_img_rect);  // 保存图片
	printf("%s written.\n%s written.\n", "./imgLeft.png", "./imgRight.png");


	/********************************显示矫正效果******************************************/
	// 创建IMG，高度一样，宽度双倍
	Mat img_origin(int(img_size.height*0.5), img_size.width, CV_8UC3);   // 矫正前的左-右视图
	Mat img_rectify(int(img_size.height*0.5), img_size.width, CV_8UC3);  // 矫正后的左-右视图

	// 浅拷贝
	Mat img_rectify_part_1 = img_rectify(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
	Mat img_rectify_part_2 = img_rectify(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

	// 填充
	cv::resize(left_img_rect, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
	cv::resize(right_img_rect, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

	// 左-右视图画横线
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		cv::line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
	}
	printf("Draw lines for rectified left-right frame done.\n");

	// 浅拷贝
	Mat img_part_1 = img_origin(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
	Mat img_part_2 = img_origin(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

	// 填充
	cv::resize(img_1, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
	cv::resize(img_2, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

	// 左-右视图画横线
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		cv::line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
	}
	printf("Draw lines for un-rectified left-right frame done.\n");

	if (show_image)
	{
		// 可视化
		cv::imshow("unrectified", img_origin);
		cv::imshow("rectified", img_rectify);
	}

	// 输出可视化结果
	cv::imwrite("./unretified.png", img_origin);
	cv::imwrite("./retified.png", img_rectify);
	printf("%s written.\n%s written.\n", "./un-retified.png", "./retified.png");
	printf("Stereo calibration done.\n");
	cout << "按任意键退出程序..." << endl;

	//if (show_image)
	//{
	//	while (1)
	//	{
	//		int a = waitKey(10);
	//		if (char(a) == 27)
	//		{
	//			break;
	//		}
	//	}
	//}

	if (show_image)
	{
		cv::waitKey(0);
	}

	return 0;
}


int readFromXmlAndRectify()
{
	/*
	Read parameters from xml file
	*/
	// ----- Read left, right intrinsic matrix
	const string l_K_xml_path("./xmls/IntrinsicMatrix_left.xml");
	const string r_K_xml_path("./xmls/IntrinsicMatrix_right.xml");

	vector<float> l_K, r_K;
	string elem_name("IntrinsicMatrix_left");
	readParamsFromXml(l_K_xml_path, elem_name, l_K);

	elem_name = "IntrinsicMatrix_right";
	readParamsFromXml(r_K_xml_path, elem_name, r_K);

	Mat l_K_mat(l_K, true);
	Mat r_K_mat(r_K, true);
	l_K_mat = l_K_mat.reshape(0, 3);
	r_K_mat = r_K_mat.reshape(0, 3);
	cout << "Left intrinsic matrix:\n" << l_K_mat << endl;
	cout << "Right intrinsic matrix:\n" << r_K_mat << endl;

	// ----- Read left and right camera distortion coefficients
	const string l_dist_xml_path("./xmls/distCoeffL.xml");
	const string r_dist_xml_path("./xmls/distCoeffR.xml");

	vector<float> l_dists, r_dists;
	elem_name = "distCoeffL";
	readParamsFromXml(l_dist_xml_path, elem_name, l_dists);

	elem_name = "distCoeffR";
	readParamsFromXml(r_dist_xml_path, elem_name, r_dists);

	Mat l_dist_mat(l_dists, true);
	Mat r_dist_mat(r_dists, true);
	l_dist_mat = l_dist_mat.reshape(0, 5);
	r_dist_mat = r_dist_mat.reshape(0, 5);
	cout << "Left distortion coefficients:\n" << l_dist_mat << endl;
	cout << "Right distortion coefficients:\n" << r_dist_mat << endl;

	// ----- Read left and right rotation matrix
	const string l_rotate_xml_path("./xmls/Rl.xml");
	const string r_rotate_xml_path("./xmls/Rr.xml");

	vector<float> l_rotate, r_rotate;
	elem_name = "Rl";
	readParamsFromXml(l_rotate_xml_path, elem_name, l_rotate);

	elem_name = "Rr";
	readParamsFromXml(r_rotate_xml_path, elem_name, r_rotate);

	Mat l_R_mat(l_rotate, true);
	Mat r_R_mat(r_rotate, true);
	l_R_mat = l_R_mat.reshape(0, 3);
	r_R_mat = r_R_mat.reshape(0, 3);
	cout << "Left rotation Mat:\n" << l_R_mat << endl;
	cout << "Right rotation Mat:\n" << r_R_mat << endl;

	// ----- Read left and right 3x4 projection matrix
	const string l_P_xml_path("./xmls/Pl.xml");
	const string r_P_xml_path("./xmls/Pr.xml");

	vector<float> l_P, r_P;
	elem_name = "Pl";
	readParamsFromXml(l_P_xml_path, elem_name, l_P);

	elem_name = "Pr";
	readParamsFromXml(r_P_xml_path, elem_name, r_P);

	Mat l_P_mat(l_P, true);
	Mat r_P_mat(r_P, true);
	l_P_mat = l_P_mat.reshape(0, 3);
	r_P_mat = r_P_mat.reshape(0, 3);
	cout << "Left projection Mat:\n" << l_P_mat << endl;
	cout << "Right projection Mat:\n" << r_P_mat << endl;

	/*
	Read image pairs, do rectifying and save
	*/
	const string img_root("F:/PyScripts/LeftRightFrames");
	const string img_rectified_root("E:/LeftRightFramesRectified");
	const string left_rectified_dir = img_rectified_root + "/image_02";
	const string right_rectified_dir = img_rectified_root + "/image_03";

	vector<string> dir_paths;
	getDirs(img_root, dir_paths);

	vector<string> left_img_paths, right_img_paths;
	getFilesFormat(dir_paths[0], ".jpg", left_img_paths);
	getFilesFormat(dir_paths[1], ".jpg", right_img_paths);
	assert(left_img_paths.size() == right_img_paths.size());

	const string left_img_dir_name("image_02");
	const string right_img_dir_name("image_03");

	string right_img_path;
	vector<string> tokens;
	int cnt = 0;
	for (const auto& left_img_path : left_img_paths)
	{
		splitStr(left_img_path, tokens, '/');
		const auto& img_name = tokens[tokens.size() - 1];

		// get corresponding right image path
		replaceStr(left_img_path, left_img_dir_name, right_img_dir_name, right_img_path, -1);
		//cout << right_img_path << endl;

		// 读取原始图像
		Mat left_img = cv::imread(left_img_path, cv::IMREAD_COLOR);
		Mat right_img = cv::imread(right_img_path, cv::IMREAD_COLOR);
		if (left_img.empty() || right_img.empty())
		{
			cout << "Left or right image is empty.\n";
			continue;
		}

		const Size& img_size = left_img.size();

		/********************************计算校正查找映射表********************************/
		// 将原图像和校正后图像上的点一一映射。
		Mat remapm_x_1 = Mat(img_size, CV_32FC1);
		Mat remapm_y_1 = Mat(img_size, CV_32FC1);
		Mat remapm_x_2 = Mat(img_size, CV_32FC1);
		Mat remapm_y_2 = Mat(img_size, CV_32FC1);

		cv::initUndistortRectifyMap(l_K_mat, l_dist_mat, l_R_mat, l_P_mat, 
			img_size, CV_16SC2, remapm_x_1, remapm_y_1);
		cv::initUndistortRectifyMap(r_K_mat, r_dist_mat, r_R_mat, r_P_mat, 
			img_size, CV_16SC2, remapm_x_2, remapm_y_2);

		// 计算矫正后的左，右视图
		Mat left_img_rectified, right_img_rectified;
		if (!remapm_x_1.empty() && !remapm_y_1.empty())  // ⑤进行矫正，映射
		{
			cv::remap(left_img, left_img_rectified, remapm_x_1, remapm_y_1, INTER_LINEAR);  // img_1 -> img_1_rectified
		}
		if (!remapm_x_2.empty() && !remapm_y_2.empty())
		{
			cv::remap(right_img, right_img_rectified, remapm_x_2, remapm_y_2, INTER_LINEAR);  // img_2 -> img_2_rectified
		}
		//if (SHOW)
		//{
		//	cv::imshow("imgLr", img_l);
		//	cv::imshow("imgRr", img_r);
		//}
		//printf("\nStereo rectifying done.\n");

		/********************************显示矫正效果******************************************/
		if (SHOW)
		{
			// 创建IMG，高度一样，宽度双倍
			Mat img_origin(int(img_size.height*0.5), img_size.width, CV_8UC3);   // 矫正前的左-右视图
			Mat img_rectify(int(img_size.height*0.5), img_size.width, CV_8UC3);  // 矫正后的左-右视图

			// 浅拷贝
			Mat img_rectify_part_1 = img_rectify(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
			Mat img_rectify_part_2 = img_rectify(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

			// resize填充
			cv::resize(left_img_rectified, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
			cv::resize(right_img_rectified, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

			// 左-右视图画横线
			for (int i = 0; i < img_rectify.rows; i += 16)
			{
				cv::line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
			}
			printf("Draw lines for rectified left-right frame done.\n");

			// 浅拷贝
			Mat img_part_1 = img_origin(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
			Mat img_part_2 = img_origin(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

			// resize填充
			cv::resize(left_img, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
			cv::resize(right_img, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // 改变图像尺寸，调节0,0

			// 左-右视图画横线
			for (int i = 0; i < img_rectify.rows; i += 16)
			{
				cv::line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
			}
			printf("Draw lines for un-rectified left-right frame done.\n");

			// 可视化
			cv::imshow("unrectified", img_origin);
			cv::imshow("rectified", img_rectify);
			cv::waitKey();
		}

		// 保存矫正之后的lefT-right图像对
		const string left_rectified_path = left_rectified_dir + '/' + img_name;
		const string right_rectified_path = right_rectified_dir + '/' + img_name;

		cv::imwrite(left_rectified_path, left_img_rectified);
		cv::imwrite(right_rectified_path, right_img_rectified);

		if (cnt % 100 == 0)
		{
			cout << left_rectified_path + " saved.\n";
			cout << right_rectified_path + " saved.\n";
		}

		cnt += 1;
	}

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
