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

// ����ȫ�ֱ���
bool image_capture = false;  // ����ȫ�ֱ�������������ɼ��궨ͼ��
bool show_image = false;      // �Ƿ���ʾ�м��ע����
char win_name_1[100];        // ����1����
char win_name_2[100];        // ����2����


//����¼���Ӧ��������������ɼ��궨ͼ��˫��������һ�βɼ�һ��ͼ��
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


int runCalibrateAndRectify()
{
	Size board_size = Size(10, 8);   // �궨���̸���ڽǵ�ߴ�(��7x7): cols, rows
	const float square_size = 20.0;  // �궨���Ϻڰ׸��ӵ�ʵ�ʱ߳���mm��
	int nFrames = 20;                // ���ڱ궨��ͼ����Ŀ
	string output_file_name;         // ����ļ�������
	bool show_undistorted = true;    // �Ƿ���ӻ��������
	vector<string> img_list_1;       // ����ͼͼ������(·��)�б�
	vector<string> img_list_2;       // ����ͼͼ������(·��)�б�
	Size img_size;                   // ����궨ͼ��ĳߴ�

	int calib_pattern = 0;  // ��ʼ��
	cout << "����һ��˫Ŀ�Ӿ��궨��������" << endl;
	cout << "���ȣ���ѡ��������궨ģʽ��1��������ͷ�ɼ��궨ͼ�񣩻�2����ͼ�����л�ȡ�궨ͼ��" << endl;
	cin >> calib_pattern;

	/************************************************************************************/
	/*********************�����ʵʱ�ɼ��궨ͼ�񲢱��浽ָ���ļ���***********************/
	//�����롰1���������������������Ԥ�����ں��������ɼ��궨ͼ��
	//��Ҫ�ɼ�ͼ�����Ŀ�ɱ���nrFrames�������ɹ���nrFrames��ͼ��Ԥ�������Զ��ر�
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

		// �������ɼ�ͼ��ķֱ��ʴ�С  
		input_capture_1.set(CAP_PROP_FRAME_WIDTH, 960);    // 640
		input_capture_1.set(CAP_PROP_FRAME_HEIGHT, 720);   // 480
		input_capture_2.set(CAP_PROP_FRAME_WIDTH, 960);
		input_capture_2.set(CAP_PROP_FRAME_HEIGHT, 720);

		if (show_image)
		{
			cv::namedWindow("�����(���1)�궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);
			cv::namedWindow("�����(���2)�궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);

			//��������¼���������������ɼ��궨ͼ����ָ������˫��������һ�βɼ�һ��ͼ��
			cv::setMouseCallback("����������1���궨ͼ��ɼ�Ԥ������", on_mouse, NULL);
		}

		//Mat src_image;
		Mat src_img_1;
		Mat src_img_2;
		int capture_count = 0;

		// ��ʱ�ɼ�: 10s���
		clock_t start_time;
		clock_t end_time;
		double  duration;
		start_time = clock();

		while (1)
		{
			input_capture_1 >> src_img_1;
			input_capture_2 >> src_img_2;

			if (show_image)  // ���������ʾ����
			{
				imshow("�����(���1)�궨ͼ��ɼ�Ԥ������", src_img_1);
				imshow("�����(���2)�궨ͼ��ɼ�Ԥ������", src_img_2);
				waitKey(50);  // 35
			}
			else  // ���������ʾ����, ÿ��10��ɼ�һ��
			{
				end_time = clock();
				duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;
				if (duration > 10)  // ���ʱ��10s
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

				// ���ñ궨ͼ����·��, ������
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
				cout << "�궨ͼ��ɼ���ϣ����ɼ���" << capture_count << "�ű궨ͼ��" << endl;
				if (show_image)
				{
					cv::destroyWindow("�����(���1)�궨ͼ��ɼ�Ԥ������");
					cv::destroyWindow("�����(���2)�궨ͼ��ɼ�Ԥ������");
				}

				image_capture = false;
				break;
			}
		}
	}

	/***********************************************************************************/
	/*******************����ű궨ͼ���·�����뵽imageList��1/2��������****************/
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
	/****************�궨ǰ��׼����������ȡObject Points��Image Points*********************/
	//1 ���ȸ���square size(���̸��ʵ�ʱ߳�������20mm)��board size(���̸�Ľǵ���������5x5)��
	// ����forѭ���õ��ǵ����������Object Points��Z�������Ϊ0��
	//2 ����forѭ����findChessboardCorners()�����õ���ǵ�������������objectPoints��Ӧ��ͼ��������������imagePoints

	vector<vector<Point2f>> img_pts_1;     // �������ͼ����ͼ���ƽ��ǵ� 
	vector<vector<Point3f>> obj_pts_1(1);  //��ʱ�ȶ���һά��obj_pts_1����ȷ����img_pts��ά��֮���ٽ�������
	vector<vector<Point2f>> img_pts_2;     // �������ͼ����ͼ���ƽ��ǵ�
	vector<vector<Point3f>> obj_pts_2(1);  // �������ͼ���пռ�ǵ�

	// ---------- ��������ͼ
	printf("\nProcessing the right frames...\n");

	// ��ȡ�ǵ����������ϵ����
	add_obj_pts(board_size, (int)square_size, obj_pts_1[0]);

	//��ͨ���ı����displayCorners1��ֵ��ȷ���Ƿ�չʾ��ȡ�ǵ���ͼ��
	bool display_corners_1 = true;

	// ��������ͼ���е�ƽ��ǵ�
	for (int i = 0; i < img_list_1.size(); ++i)
	{
		Mat src_1 = imread(img_list_1[i], 1);
		img_size = src_1.size();
		vector<Point2f> pts_buff_1;

		//ʹ�ò�ͬ��FLAG�������궨�������С
		bool found_1 = cv::findChessboardCorners(src_1, board_size, pts_buff_1);
		//bool found_1 = findChessboardCorners(src1, board_size, pt_buff_1, CALIB_CB_ADAPTIVE_THRESH);
		//bool found_1 = findChessboardCorners(src1, board_size, pt_buff_1, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);  // ��һ���ȽϷ�ʱ

		if (found_1)
		{
			Mat gray_img_1;
			cv::cvtColor(src_1, gray_img_1, COLOR_BGR2GRAY);

			cv::cornerSubPix(gray_img_1,
				pts_buff_1,
				Size(11, 11),
				Size(-1, -1),
				TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.1));

			// ��ŵ�ǰ����ͼƽ��ǵ�
			img_pts_1.push_back(pts_buff_1);

			if (show_image & display_corners_1)
			{
				sprintf(win_name_1, "������ǵ��ȡ���");
				Mat src_show_1 = src_1.clone();
				cv::drawChessboardCorners(src_show_1, board_size, Mat(pts_buff_1), found_1);
				cv::imshow(win_name_1, src_show_1);
				cv::waitKey(500);
				cv::destroyWindow(win_name_1);
			}
		}

		printf("[Left frame %d] finding corners done.\n", i + 1);
	}

	// ��object points������������
	obj_pts_1.resize(img_pts_1.size(), obj_pts_1[0]);  // ����(����ͼͼ�����)��

	// ---------- ��������ͼ
	printf("\nProcessing the right frames...\n");

	// ��ȡ�ǵ����������ϵ����
	add_obj_pts(board_size, (int)square_size, obj_pts_2[0]);

	bool display_corners_2 = true;
	for (int i = 0; i < img_list_2.size(); i++)
	{
		Mat src_2 = imread(img_list_2[i], 1);
		vector<Point2f> pts_buff_2;

		//ʹ�ò�ͬ��FLAG�������궨�������С
		bool found_2 = cv::findChessboardCorners(src_2, board_size, pts_buff_2);
		//bool found_2 = findChessboardCorners(src_2, board_size, pts_buff_2, CALIB_CB_ADAPTIVE_THRESH);
		//bool found_2 = findChessboardCorners(src_2, board_size, pts_buff_2, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);  // ��һ���ȽϷ�ʱ

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
				sprintf(win_name_1, "������ǵ��ȡ���");

				Mat src_show_2 = src_2.clone();
				cv::drawChessboardCorners(src_show_2, board_size, Mat(pts_buff_2), found_2);
				cv::imshow(win_name_1, src_show_2);
				cv::waitKey(500);
				cv::destroyWindow(win_name_1);
			}
		}

		printf("[Right frame %d] finding corners done.\n", i + 1);
	}

	// ������ͼ�Ŀռ�ǵ�������������
	obj_pts_2.resize(img_pts_2.size(), obj_pts_2[0]);

	/**************************************************************************************/
	/********************************���е�Ŀ����궨******************************************/
	//ͨ��calibrateCamera()������������궨
	//��ҪΪ�˵õ�������ڲξ���cameraMatrix������ϵ������distCoeffs
	//�������ͨ���������ص���ͶӰ����С��������궨�ľ������
	//����õ��������ξ�����Ҫ

	//Mat camera_matrix = Mat::eye(3, 3, CV_64F);
	//Mat distCoeffs = Mat::zeros(8, 1, CV_64F);   // ����ϵ����˳����[k1,k2,p1,p2,k3,(k4,k5,k6)]
	Mat camera_matrix_1 = Mat::eye(3, 3, CV_64F);  // ����ͼ����ڲξ���
	Mat dist_coeffs_1 = Mat::zeros(5, 1, CV_64F);  // ����ͼ����ϵ�� 
	Mat camera_matrix_2 = Mat::eye(3, 3, CV_64F);  // ����ͼ����ڲξ���
	Mat dist_coeffs_2 = Mat::zeros(5, 1, CV_64F);  // ����ͼ����ϵ��

	//vector<Mat> rvecs, tvecs;
	vector<Mat> r_vecs_1, t_vecs_1;  // ����ͼÿ����ͼ��λ��: ��ת�����ƽ������ 
	vector<Mat> r_vecs_2, t_vecs_2;  // ����ͼÿ����ͼ��λ��: ��ת�����ƽ������ 

	/*
	double re_project_err=calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs,0);
	//checkRange()����---���ڼ������е�ÿһ��Ԫ���Ƿ���ָ����һ����ֵ����֮��
	bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);
	if(ok)
	{
		cout<<"����궨�ɹ���"<<endl;
		cout<<"����궨����ͶӰ��"<<re_project_err<<endl;
		cout<<"����ڲξ���"<<endl<<cameraMatrix<<endl;
		cout<<"�������ϵ������"<<endl<<distCoeffs<<endl;
		if(showUndistorsed==true)
		{
			for(int i=0;i<imageList.size();i++)
			{
				Mat temp=imread(imageList1[i],1);;
				//����undistort()�����õ��������������ͼ��
				Mat undistort_view;
				undistort(temp, undistort_view, cameraMatrix, distCoeffs);
				imshow("ԭ����ͼ��",temp);
				imshow("�������ͼ��",undistort_view);
				waitKey(500);
			}
			destroyWindow("ԭ����ͼ��");
			destroyWindow("�������ͼ��");
		}
	}
	*/
	/*
	calibrateCamera()
	������� objectPoints  �ǵ��ʵ����������
			 imagePoints   �ǵ��ͼ������
			 imageSize     ͼ��Ĵ�С
	�������
			 cameraMatrix  ������ڲξ���
			 distCoeffs    ����Ļ������
			 rvecs         ��תʸ��(�����)
			 tvecs         ƽ��ʸ��(�������
	*/

	//����ϵ����˳����[k1,k2,p1,p2,k3,(k4,k5,k6)]
	//������ǲ���ҪK3���ڳ�ʼ��K3ΪO֮�󣬿���ʹ�ñ�־CV_CALIB_FIX_K3���������궨��������ı�K3��ֵ
	//һ��أ�K3Ӧ����Ϊ0������ʹ�����۾�ͷ���ο���learning opencv����ʮһ�£�
	//���ص�distCoeffs1�����ĳ����ɱ�־λflag��������flag����ΪCV_CALIB_RATIONAL_MODELʱ�������л��������8����
	//�����ó�����flagʱ������5ά�Ļ���ϵ������[k1, k2, p1, p2, k3]

	printf("\nStart calibrate the left camera...\n");
	double re_proj_err_1 = cv::calibrateCamera(obj_pts_1,
		img_pts_1,
		img_size,
		camera_matrix_1,
		dist_coeffs_1,
		r_vecs_1,
		t_vecs_1,
		CALIB_FIX_K3);

	//checkRange()����  ---���ڼ������е�ÿһ��Ԫ�ص���Ч��
	bool ok_1 = cv::checkRange(camera_matrix_1) && checkRange(dist_coeffs_1);
	if (ok_1)
	{
		printf("\n");
		cout << "������궨�ɹ���" << endl;
		cout << "������궨����ͶӰ��" << re_proj_err_1 << "pixel" << endl;
		cout << "������ڲξ���" << endl << camera_matrix_1 << endl;
		cout << "���������ϵ������" << endl << dist_coeffs_1 << endl;

		if (show_undistorted == true)
		{
			for (int i = 0; i < img_list_1.size(); i++)
			{
				Mat temp = imread(img_list_1[i], 1);

				//����undistort()�����õ��������������ͼ��
				Mat undistort_view;
				cv::undistort(temp, undistort_view, camera_matrix_1, dist_coeffs_1);

				if (show_image)
				{
					sprintf(win_name_1, "ԭ����ͼ��");
					sprintf(win_name_2, "�������ͼ��");
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
		cout << "������궨�ɹ���" << endl;
		cout << "������궨����ͶӰ��" << re_project_err_2 << "pixel" << endl;
		cout << "������ڲξ���" << endl << camera_matrix_2 << endl;
		cout << "���������ϵ������" << endl << dist_coeffs_2 << endl;

		if (show_undistorted == true)
		{
			for (int i = 0; i < img_list_2.size(); i++)
			{
				Mat temp = imread(img_list_2[i], 1);

				//����undistort()�����õ��������������ͼ��
				Mat undistort_view;

				cv::undistort(temp, undistort_view, camera_matrix_2, dist_coeffs_2);

				if (show_image)
				{
					sprintf(win_name_1, "ԭ����ͼ��");
					sprintf(win_name_2, "�������ͼ��");

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
	/********************************����궨(˫Ŀ�궨)******************************************/
	//����stereoCalibrate()������������궨���õ�4������R ��תʸ�� Tƽ��ʸ�� E�������� F��������
	//ͬʱ���Եõ�����������ڲξ���ͻ���ϵ������cameraMatrix1��distCoeffs1��cameraMatrix2��distCoeffs2
	//Ҳ����˵��ʵ���Բ������ȵ�����ÿ��������б궨��
	//stereoCalibrate()�����ڲ�Ӧ���ǵ�����calibrateCamera()������
	//����Ҳ�����ȵ���calibrateCamera()�����ȶ�ÿ��������б궨���õ��ڲξ���ͻ���ϵ���ĳ�ʼֵ
	//Ȼ������궨ʱstereoCalibrate()��������ڲξ���ͻ���ϵ�������Ż�����ʱӦ����flag��CALIB_USE_INTRINSIC_GUESS
	//���û�����ȵ��ù�calibrateCamera()�������벻Ҫʹ��flag:CALIB_USE_INTRINSIC_GUESS����õ�����ֵĽ��
	//���֮ǰ�궨��������ڲξ���ͻ�����������⣬����������궨ʱ����һ���Ż�����ʹ��CV_CALIB_FIX_INTRINSIC
	//���ݹٷ��ĵ����飬stereoCalibrate()��������Ĳ����ռ��ά���ܸߣ�һ���Եõ��ܶ�����
	//���ܻᵼ��ĳЩ�����ɢ���������ֵ��ƫ����ȷ����������ǰʹ����calibrateCamera()������ÿ��������й��궨
	//�����ѡ��CALIB_FIX_INTRINSICӦ�õ�stereoCalibrate()�����У������ܼ��ټ���Ĳ���
	//��ֹ����ĳЩ�����ɢ���������ֵ
	//CV_CALIB_FIX_INTRINSIC��������Ƿ�ʹ�û����������һ��Ȩ��

	printf("\nStart stereo calibrating...\n");

	//R: ��תʸ�� T: ƽ��ʸ�� E: �������� F: ��������
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
	cout << "������ڲξ���" << endl << camera_matrix_1 << endl;
	cout << "���������ϵ������" << endl << dist_coeffs_1 << endl;
	cout << "������ڲξ���" << endl << camera_matrix_2 << endl;
	cout << "���������ϵ������" << endl << dist_coeffs_2 << endl;
	cout << "R:" << endl << R << endl;
	cout << "T:" << endl << T << endl;
	printf("Stereo calibrating done.\n");

	/**************************************************************************************/
	/********************************����궨���******************************************/
	//����.xml�ļ�ʱ��Ҫע��2�����⣺
	//1 ��Ҫ�����Mat�ͱ�������ʱ����Ҫ��ʼ������������������
	//2 ����ʱ�����ı�ʶ�������в��ܳ��֡�.����

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

	/********************************�������********************************/
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

	/********************************����У������ӳ���********************************/
	// ��ԭͼ���У����ͼ���ϵĵ�һһӳ�䡣
	Mat remapm_x_1 = Mat(img_size, CV_32FC1);
	Mat remapm_y_1 = Mat(img_size, CV_32FC1);
	Mat remapm_x_2 = Mat(img_size, CV_32FC1);
	Mat remapm_y_2 = Mat(img_size, CV_32FC1);

	cv::initUndistortRectifyMap(camera_matrix_1, dist_coeffs_1, R1, P1, img_size, CV_16SC2, remapm_x_1, remapm_y_1);
	cv::initUndistortRectifyMap(camera_matrix_2, dist_coeffs_2, R2, P2, img_size, CV_16SC2, remapm_x_2, remapm_y_2);

	// ��ȡԭʼͼ
	Mat img_1 = cv::imread(img_list_1[0], IMREAD_COLOR);
	Mat img_2 = cv::imread(img_list_2[0], IMREAD_COLOR);

	// ����������������ͼ
	Mat left_img_rect, right_img_rect;  
	if (!remapm_x_1.empty() && !remapm_y_1.empty())  // �ݽ��н�����ӳ��
	{
		cv::remap(img_1, left_img_rect, remapm_x_1, remapm_y_1, INTER_LINEAR);  // img_1 -> img_l
	}
	if (!remapm_x_2.empty() && !remapm_y_2.empty())
	{
		cv::remap(img_2, right_img_rect, remapm_x_2, remapm_y_2, INTER_LINEAR);  // img_2 -> img_r
	}
	if (show_image)  // ���ӻ�
	{	
		cv::imshow("imgLr", left_img_rect);
		cv::imshow("imgRr", right_img_rect);
	}
	printf("Stereo rectifying done.\n");

	cv::imwrite("./imgLeft.png", left_img_rect);
	cv::imwrite("./imgRight.png", right_img_rect);  // ����ͼƬ
	printf("%s written.\n%s written.\n", "./imgLeft.png", "./imgRight.png");


	/********************************��ʾ����Ч��******************************************/
	// ����IMG���߶�һ�������˫��
	Mat img_origin(int(img_size.height*0.5), img_size.width, CV_8UC3);   // ����ǰ����-����ͼ
	Mat img_rectify(int(img_size.height*0.5), img_size.width, CV_8UC3);  // ���������-����ͼ

	// ǳ����
	Mat img_rectify_part_1 = img_rectify(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
	Mat img_rectify_part_2 = img_rectify(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

	// ���
	cv::resize(left_img_rect, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
	cv::resize(right_img_rect, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

	// ��-����ͼ������
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		cv::line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
	}
	printf("Draw lines for rectified left-right frame done.\n");

	// ǳ����
	Mat img_part_1 = img_origin(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
	Mat img_part_2 = img_origin(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

	// ���
	cv::resize(img_1, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
	cv::resize(img_2, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

	// ��-����ͼ������
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		cv::line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
	}
	printf("Draw lines for un-rectified left-right frame done.\n");

	if (show_image)
	{
		// ���ӻ�
		cv::imshow("unrectified", img_origin);
		cv::imshow("rectified", img_rectify);
	}

	// ������ӻ����
	cv::imwrite("./unretified.png", img_origin);
	cv::imwrite("./retified.png", img_rectify);
	printf("%s written.\n%s written.\n", "./un-retified.png", "./retified.png");
	printf("Stereo calibration done.\n");
	cout << "��������˳�����..." << endl;

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

		// ��ȡԭʼͼ��
		Mat left_img = cv::imread(left_img_path, cv::IMREAD_COLOR);
		Mat right_img = cv::imread(right_img_path, cv::IMREAD_COLOR);
		if (left_img.empty() || right_img.empty())
		{
			cout << "Left or right image is empty.\n";
			continue;
		}

		const Size& img_size = left_img.size();

		/********************************����У������ӳ���********************************/
		// ��ԭͼ���У����ͼ���ϵĵ�һһӳ�䡣
		Mat remapm_x_1 = Mat(img_size, CV_32FC1);
		Mat remapm_y_1 = Mat(img_size, CV_32FC1);
		Mat remapm_x_2 = Mat(img_size, CV_32FC1);
		Mat remapm_y_2 = Mat(img_size, CV_32FC1);

		cv::initUndistortRectifyMap(l_K_mat, l_dist_mat, l_R_mat, l_P_mat, 
			img_size, CV_16SC2, remapm_x_1, remapm_y_1);
		cv::initUndistortRectifyMap(r_K_mat, r_dist_mat, r_R_mat, r_P_mat, 
			img_size, CV_16SC2, remapm_x_2, remapm_y_2);

		// ����������������ͼ
		Mat left_img_rectified, right_img_rectified;
		if (!remapm_x_1.empty() && !remapm_y_1.empty())  // �ݽ��н�����ӳ��
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

		/********************************��ʾ����Ч��******************************************/
		if (SHOW)
		{
			// ����IMG���߶�һ�������˫��
			Mat img_origin(int(img_size.height*0.5), img_size.width, CV_8UC3);   // ����ǰ����-����ͼ
			Mat img_rectify(int(img_size.height*0.5), img_size.width, CV_8UC3);  // ���������-����ͼ

			// ǳ����
			Mat img_rectify_part_1 = img_rectify(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
			Mat img_rectify_part_2 = img_rectify(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

			// resize���
			cv::resize(left_img_rectified, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
			cv::resize(right_img_rectified, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

			// ��-����ͼ������
			for (int i = 0; i < img_rectify.rows; i += 16)
			{
				cv::line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
			}
			printf("Draw lines for rectified left-right frame done.\n");

			// ǳ����
			Mat img_part_1 = img_origin(Rect(0, 0, int(img_size.width*0.5), int(img_size.height*0.5)));
			Mat img_part_2 = img_origin(Rect(int(img_size.width*0.5), 0, int(img_size.width*0.5), int(img_size.height*0.5)));

			// resize���
			cv::resize(left_img, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
			cv::resize(right_img, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

			// ��-����ͼ������
			for (int i = 0; i < img_rectify.rows; i += 16)
			{
				cv::line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
			}
			printf("Draw lines for un-rectified left-right frame done.\n");

			// ���ӻ�
			cv::imshow("unrectified", img_origin);
			cv::imshow("rectified", img_rectify);
			cv::waitKey();
		}

		// �������֮���lefT-rightͼ���
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


/*����궨����ģ���ʵ����������*/
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


int main(int argc, char* argv[])
{
	//runCalibrateAndRectify();

	readFromXmlAndRectify();
}
