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


bool image_capture = false;//����ȫ�ֱ�������������ɼ��궨ͼ��
//����¼���Ӧ��������������ɼ��궨ͼ��˫��������һ�βɼ�һ��ͼ��
void on_mouse(int event, int x, int y, int flags, void* a)
{
	if (event == EVENT_LBUTTONDBLCLK)
	{
		image_capture = true;
	}

}


int main(int argc, char* argv[])
{
	Size board_size = Size(10, 8);  // �궨���̸���ڽǵ�ߴ�(��7x7): cols, rows
	float square_size = 20.0;        // �궨���Ϻڰ׸��ӵ�ʵ�ʱ߳���mm��
	int nFrames = 20;               // ���ڱ궨��ͼ����Ŀ
	string outputFileName;          // ����ļ�������
	bool showUndistorsed = true;
	//vector<string> imageList;
	vector<string> img_list_1;
	vector<string> img_list_2;
	Size img_size;
	//Size img_size_1;
	//Size img_size_2;

	int calib_pattern = 0;
	cout << "����һ��˫Ŀ�Ӿ�����" << endl;
	cout << "���ȣ���ѡ��������궨ģʽ��1��������ͷ�ɼ��궨ͼ�񣩻�2����ͼ�����л�ȡ�궨ͼ��" << endl;
	cin >> calib_pattern;

	/************************************************************************************/
	/*********************�����ʵʱ�ɼ��궨ͼ�񲢱��浽ָ���ļ���***********************/
	//�����롰1���������������������Ԥ�����ں��������ɼ��궨ͼ��
	//��Ҫ�ɼ�ͼ�����Ŀ�ɱ���nrFrames�������ɹ���nrFrames��ͼ��Ԥ�������Զ��ر�
	if (calib_pattern == 1)
	{
		//VideoCapture inputCapture;
		VideoCapture inputCapture1;
		VideoCapture inputCapture2;
		//inputCapture.open(1);
		inputCapture2.open(2);
		inputCapture1.open(1);

		//if(!inputCapture.isOpened()==true) return -1;
		if (!inputCapture1.isOpened() == true) return -1;
		if (!inputCapture2.isOpened() == true) return -1;

		//inputCapture.set(CV_CAP_PROP_FRAME_WIDTH, 640);  
	   // inputCapture.set(CV_CAP_PROP_FRAME_HEIGHT, 480); 

		inputCapture1.set(CAP_PROP_FRAME_WIDTH, 960);//�������ɼ�ͼ��ķֱ��ʴ�С  
		inputCapture1.set(CAP_PROP_FRAME_HEIGHT, 720);
		inputCapture2.set(CAP_PROP_FRAME_WIDTH, 960);
		inputCapture2.set(CAP_PROP_FRAME_HEIGHT, 720);

		/*
		inputCapture1.set(CV_CAP_PROP_FRAME_WIDTH, 640);//�������ɼ�ͼ��ķֱ��ʴ�С
		inputCapture1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		inputCapture2.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		inputCapture2.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		*/
		//namedWindow("�궨ͼ��ɼ�Ԥ������",CV_WINDOW_AUTOSIZE);
		cv::namedWindow("����������1���궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);
		cv::namedWindow("����������2���궨ͼ��ɼ�Ԥ������", WINDOW_AUTOSIZE);

		//��������¼���������������ɼ��궨ͼ����ָ������˫��������һ�βɼ�һ��ͼ��
		//cvSetMouseCallback("�궨ͼ��ɼ�Ԥ������",on_mouse,NULL);
		cv::setMouseCallback("����������1���궨ͼ��ɼ�Ԥ������", on_mouse, NULL);

		//Mat src_image;
		Mat src_img_1;
		Mat src_img_2;
		int capture_count = 0;

		while (1)
		{
			//inputCapture>>src_image;
			inputCapture1 >> src_img_1;
			inputCapture2 >> src_img_2;

			//imshow("�궨ͼ��ɼ�Ԥ������",src_image);
			imshow("����������1���궨ͼ��ɼ�Ԥ������", src_img_1);
			imshow("����������2���궨ͼ��ɼ�Ԥ������", src_img_2);
			waitKey(35);
			if (image_capture == true && capture_count < nFrames)
			{
				//Mat cap;
				Mat cap1;
				Mat cap2;
				//inputCapture>>cap;
				inputCapture1 >> cap1;
				inputCapture2 >> cap2;
				char address[100];

				//ƴ�ձ궨ͼ����·��������
				//sprintf(address,"Calibration_Image_Camera\\Image%d%s",capture_count+1,".jpg");
				//imwrite(address,cap);
				sprintf(address, "Calibration_Image_Camera\\Image_l%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap1);
				sprintf(address, "Calibration_Image_Camera\\Image_r%d%s", capture_count + 1, ".jpg");
				imwrite(address, cap2);
				capture_count++;
				image_capture = false;
			}
			else if (capture_count >= nFrames)
			{
				cout << "�궨ͼ��ɼ���ϣ����ɼ���" << capture_count << "�ű궨ͼ��" << endl;
				//destroyWindow("�궨ͼ��ɼ�Ԥ������");
				destroyWindow("����������1���궨ͼ��ɼ�Ԥ������");
				destroyWindow("����������2���궨ͼ��ɼ�Ԥ������");
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
			//sprintf(name,"Calibration_Image_Camera/Image%d%s",i,".jpg");
			//imageList.push_back(name);
			sprintf(name, "Calibration_Image_Camera1/Image_l%d%s", i, ".jpg");
			img_list_1.push_back(name);
			sprintf(name, "Calibration_Image_Camera1/Image_r%d%s", i, ".jpg");
			img_list_2.push_back(name);
		}
	}
	//cout<<"ImageList.size:"<<imageList.size()<<endl;
	cout << "Image list 1 size:" << img_list_1.size() << endl;
	cout << "Image list 2 size:" << img_list_2.size() << endl;

	/************************************************************************************/
	/****************�궨ǰ��׼����������ȡObject Points��Image Points*********************/
	//1 ���ȸ���square size(���̸��ʵ�ʱ߳�������20mm)��board size(���̸�Ľǵ���������5x5)��
	// ����forѭ���õ��ǵ����������Object Points��Z�������Ϊ0��
	//2 ����forѭ����findChessboardCorners()�����õ���ǵ�������������objectPoints��Ӧ��ͼ��������������imagePoints

	//vector<vector<Point2f> > imagePoints;//����ͼ���ҵ��Ľǵ�ļ���
	//vector<vector<Point3f> > objectPoints(1);
	vector<vector<Point2f>> img_pts_1;     // �������ͼ����ͼ���ƽ��ǵ� 
	vector<vector<Point3f>> obj_pts_1(1);  //��ʱ�ȶ���һά��obj_pts_1����ȷ����img_pts��ά��֮���ٽ�������
	vector<vector<Point2f>> img_pts_2;     // �������ͼ����ͼ���ƽ��ǵ�
	vector<vector<Point3f>> obj_pts_2(1);  // �������ͼ���пռ�ǵ�
	/*
	//����calobjectPoints()�������ڵõ����̸�ǵ���������꼯
	calobjectPoints(objectPoints[0], boardSize,squareSize);

	//��ͨ���ı����showChessboardCorner��ֵ��ȷ���Ƿ�չʾ��ȡ�ǵ���ͼ��
	bool displayCorners = false;
	for(int i=0;i<imageList.size();i++)
	{
		Mat src=imread(imageList[i],1);
		//imshow("��ʾ",src);
		imageSize = src.size();
		vector<Point2f> pointBuf;//ĳһ��ͼ���ҵ��Ľǵ�
		bool found=findChessboardCorners( src, boardSize, pointBuf, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);//��һ���ȽϷ�ʱ
		if(found)
		{
			Mat grayimage;
			cvtColor(src, grayimage, COLOR_BGR2GRAY);
			cornerSubPix( grayimage, pointBuf, Size(11,11),Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.1 ));
			imagePoints.push_back(pointBuf);
			if(displayCorners)
			{
				Mat MidImage=src.clone();
				drawChessboardCorners( MidImage, boardSize, Mat(pointBuf), found );
				imshow("�ǵ��ȡ���",MidImage);
				waitKey(300);
			}
			destroyWindow("�ǵ��ȡ���");
		}
	}
	objectPoints.resize(imagePoints.size(), objectPoints[0]);
	*/

	// ---------- ��������ͼ
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

			if (display_corners_1)
			{
				Mat src_show_1 = src_1.clone();
				cv::drawChessboardCorners(src_show_1, board_size, Mat(pts_buff_1), found_1);
				cv::imshow("������ǵ��ȡ���", src_show_1);
				cv::waitKey(300);
			}

			cv::destroyWindow("������ǵ��ȡ���");
		}
	}

	// ��object points������������
	obj_pts_1.resize(img_pts_1.size(), obj_pts_1[0]);  // ����(����ͼͼ�����)��

	// ---------- ��������ͼ
	add_obj_pts(board_size, (int)square_size, obj_pts_2[0]);

	bool display_corners_2 = true;
	for (int i = 0; i < img_list_2.size(); i++)
	{
		Mat src_2 = imread(img_list_2[i], 1);
		//imageSize= src2.size();
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
			if (display_corners_2)
			{
				Mat src_show_2 = src_2.clone();
				cv::drawChessboardCorners(src_show_2, board_size, Mat(pts_buff_2), found_2);
				cv::imshow("������ǵ��ȡ���", src_show_2);
				cv::waitKey(300);
			}

			cv::destroyWindow("������ǵ��ȡ���");
		}
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
				waitKey(300);
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
	//�����ó�����flagʱ������5ά�Ļ���ϵ������[k1,k2,p1,p2,k3]

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
		cout << "������궨�ɹ���" << endl;
		cout << "������궨����ͶӰ��" << re_project_err_2 << "pixel" << endl;
		//cout << "������ڲξ���" << endl << camera_matrix_2 << endl;
		//cout << "���������ϵ������" << endl << dist_coeffs_2 << endl;
	}

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
	//�����ѡ��CV_CALIB_FIX_INTRINSICӦ�õ�stereoCalibrate()�����У������ܼ��ټ���Ĳ���
	//��ֹ����ĳЩ�����ɢ���������ֵ
	//CV_CALIB_FIX_INTRINSIC��������Ƿ�ʹ�û����������һ��Ȩ��

	//R: ��תʸ�� T: ƽ��ʸ�� E: �������� F: ��������
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
	cout << "������ڲξ���" << endl << camera_matrix_1 << endl;
	cout << "���������ϵ������" << endl << dist_coeffs_1 << endl;
	cout << "������ڲξ���" << endl << camera_matrix_2 << endl;
	cout << "���������ϵ������" << endl << dist_coeffs_2 << endl;
	cout << "R:" << endl << R << endl;
	cout << "T:" << endl << T << endl;

	/**************************************************************************************/
	/********************************����궨���******************************************/
	//����.xml�ļ�ʱ��Ҫע��2�����⣺
	//1 ��Ҫ�����Mat�ͱ�������ʱ����Ҫ��ʼ������������������
	//2 ����ʱ�����ı�ʶ�������в��ܳ��֡�.����

	const string calib_f_path = "./Calibration_Result.xml";
	//string calib_f_path = "Calibration_Result.xml";
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

	/********************************����У������ӳ���********************************/
	// ��ԭͼ���У����ͼ���ϵĵ�һһӳ�䡣
	Mat remapm_x_1 = Mat(img_size, CV_32FC1);
	Mat remapm_y_1 = Mat(img_size, CV_32FC1);
	Mat remapm_x_2 = Mat(img_size, CV_32FC1);
	Mat remapm_y_2 = Mat(img_size, CV_32FC1);

	cv::initUndistortRectifyMap(camera_matrix_1, dist_coeffs_1, R1, P1, img_size, CV_16SC2, remapm_x_1, remapm_y_1);
	cv::initUndistortRectifyMap(camera_matrix_2, dist_coeffs_2, R2, P2, img_size, CV_16SC2, remapm_x_2, remapm_y_2);

	Mat img_1 = imread(img_list_1[0], IMREAD_COLOR);
	Mat img_2 = imread(img_list_2[0], IMREAD_COLOR);
	Mat img_l, img_r;  // �������������ͼ
	if (!remapm_x_1.empty() && !remapm_y_1.empty())  // �ݽ��н�����ӳ��
	{
		remap(img_1, img_l, remapm_x_1, remapm_y_1, INTER_LINEAR);
	}
	if (!remapm_x_2.empty() && !remapm_y_2.empty())
	{
		remap(img_2, img_r, remapm_x_2, remapm_y_2, INTER_LINEAR);
	}
	imshow("imgLr", img_l);
	imshow("imgRr", img_r);

	imwrite("./imgLeft.png", img_l);
	imwrite("./imgRight.png", img_r);  // ����ͼƬ

	/********************************��ʾ����Ч��******************************************/
	// ����IMG���߶�һ�������˫��
	Mat img_origin(img_size.height*0.5, img_size.width, CV_8UC3);   // ����ǰ����-����ͼ
	Mat img_rectify(img_size.height*0.5, img_size.width, CV_8UC3);  // ���������-����ͼ

	// ǳ����
	Mat img_rectify_part_1 = img_rectify(Rect(0, 0, img_size.width*0.5, img_size.height*0.5));  
	Mat img_rectify_part_2 = img_rectify(Rect(img_size.width*0.5, 0, img_size.width*0.5, img_size.height*0.5));

	// ���
	resize(img_l, img_rectify_part_1, img_rectify_part_1.size(), 0, 0, INTER_AREA);
	resize(img_r, img_rectify_part_2, img_rectify_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

	// ��-����ͼ������
	for (int i = 0; i < img_rectify.rows; i += 16)  
	{
		line(img_rectify, Point(0, i), Point(img_rectify.cols, i), Scalar(0, 255, 0), 1, 8);
	}

	// ǳ����
	Mat img_part_1 = img_origin(Rect(0, 0, img_size.width*0.5, img_size.height*0.5));
	Mat img_part_2 = img_origin(Rect(img_size.width*0.5, 0, img_size.width*0.5, img_size.height*0.5));

	// ���
	resize(img_1, img_part_1, img_part_1.size(), 0, 0, INTER_AREA);
	resize(img_2, img_part_2, img_part_2.size(), 0, 0, INTER_AREA);  // �ı�ͼ��ߴ磬����0,0

	// ��-����ͼ������
	for (int i = 0; i < img_rectify.rows; i += 16)
	{
		line(img_origin, Point(0, i), Point(img_origin.cols, i), Scalar(0, 255, 0), 1, 8);
	}

	// ���ӻ�
	imshow("un-rectified", img_origin);
	imshow("rectified", img_rectify);

	// ������ӻ����
	imwrite("un-retified.png", img_origin);
	imwrite("retified.png", img_rectify);

	cout << "��������˳�����..." << endl;

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