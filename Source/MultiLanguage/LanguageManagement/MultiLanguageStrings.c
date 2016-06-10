#include "MultiLanguageStrings.h"
#include "SSD1306.h"

const char SystemInit_EN[] = "System Init...";
const char SystemInit_TW[] = "�t�Ϊ�l��...";
const char SystemInit_JC[] = "ϵͳ��ʼ��...";
const char SystemInit_JP[] = "�V�X�e���̏�����...";
const char * SystemInit_Str[] = { SystemInit_EN,SystemInit_TW,SystemInit_JC,SystemInit_JP };

const char Capacity_EN[] = "Capacity:%dMB";
const char Capacity_TW[] = "�e�q:%dMB";
const char Capacity_JC[] = "����:%dMB";
const char Capacity_JP[] = "�e��:%dMB";
const char * Capacity_Str[] = { Capacity_EN,Capacity_TW,Capacity_JC,Capacity_JP };

const char NoSD_EN[] = "No SDCard";
const char NoSD_TW[] = "�S���O����";
const char NoSD_JC[] = "û��SD��";
const char NoSD_JP[] = "������TF�J�[�h";
const char * NoSD_Str[] = { NoSD_EN,NoSD_TW,NoSD_JP };

const char WaitingForEBD_EN[] = "Waiting for EBD...";
const char WaitingForEBD_TW[] = "���ݳ]��...";
const char WaitingForEBD_JC[] = "�ȴ��豸...";
const char WaitingForEBD_JP[] = "�ڑ����܂�...";
const char * WaitingForEBD_Str[] = { WaitingForEBD_EN,WaitingForEBD_TW,WaitingForEBD_JC,WaitingForEBD_JP };

const char EBDConnected_EN[] = "EBD Connected";
const char EBDConnected_TW[] = "�]�Ƥw�s��";
const char EBDConnected_JC[] = "�豸������";
const char EBDConnected_JP[] = "�ڑ����ł�";
const char * EBDConnected_Str[] = { EBDConnected_EN,EBDConnected_TW,EBDConnected_JC,EBDConnected_JP };

const char Confirmation_EN[] = "Confirmation";
const char Confirmation_TW[] = "�T�{";
const char Confirmation_JC[] = "ȷ��";
const char Confirmation_JP[] = "�m�F���܂�";
const char * Confirmation_Str[] = { Confirmation_EN,Confirmation_TW,Confirmation_JC,Confirmation_JP };

const char ConfirmCancel_EN[] = "Cancel%Comfirm";
const char ConfirmCancel_TW[] = " ���� % �T�{ ";
const char ConfirmCancel_JC[] = " ȡ�� % ȷ�� " ;
const char ConfirmCancel_JP[] = " �L�����Z�� % �͂� ";
const char * ConfirmCancel_Str[] = { ConfirmCancel_EN,ConfirmCancel_TW,ConfirmCancel_JC,ConfirmCancel_JP };

const char RecordConfirm_EN[] = "Start Record?";
const char RecordConfirm_TW[] = "�}�l�O�����u? ";
const char RecordConfirm_JC[] = "��ʼ��¼����?" ;
const char RecordConfirm_JP[] = "�Ȑ����R�[�h?";
const char * RecordConfirm_Str[] = { RecordConfirm_EN,RecordConfirm_TW,RecordConfirm_JC,RecordConfirm_JP };

const char QCMTKConfirm_EN[] = "QC/MTK Trigger?";
const char QCMTKConfirm_TW[] = "QC/MTKĲ�o? ";
const char QCMTKConfirm_JC[] = "QC/MTK����?" ;
const char QCMTKConfirm_JP[] = "QC/MTK�g���K�[?";
const char * QCMTKConfirm_Str[] = { QCMTKConfirm_EN,QCMTKConfirm_TW,QCMTKConfirm_JC,QCMTKConfirm_JP };

const char Saved_EN[] = "Saved";
const char Saved_TW[] = "�w�O�s";
const char Saved_JC[] = "�ѱ���" ;
const char Saved_JP[] = "�ۑ�";
const char * Saved_Str[] = { Saved_EN,Saved_TW,Saved_JC,Saved_JP };

const char Settings_EN[]="Unmount Disk%Clock Settings%Buzzer Settings%Language%System Scan%System Info";
const char Settings_TW[]="�u�XSD�d%�ɶ��]�m%ĵ���]�m%Language%�t�α��y%�t�θ�T";
const char Settings_JC[] = "��ȫ�����洢��%ʱ������%����������%Language%ϵͳɨ��%ϵͳ��Ϣ" ;
const char Settings_JP[] ="�C�W�F�N�g������%�����̐ݒ�%�x���̐ݒ�%Language%�V�X�e���X�L����%�V�X�e�����";
const char * Settings_Str[] = { Settings_EN,Settings_TW,Settings_JC,Settings_JP };

const char SetYear_EN[]="Set year";
const char SetYear_TW[]="�]�m�~��";
const char SetYear_JC[] = "�������" ;
const char SetYear_JP[] ="�N��ݒ�";
const char * SetYear_Str[] = { SetYear_EN,SetYear_TW,SetYear_JC,SetYear_JP };

const char SetYearUnit_EN[] ="Year";
const char SetYearUnit_TW[] ="�~";
const char SetYearUnit_JC[] ="��" ;
const char SetYearUnit_JP[] ="�N";
const char * SetYearUnit_Str[] = { SetYearUnit_EN,SetYearUnit_TW,SetYearUnit_JC,SetYearUnit_JP };

const char SetMonth_EN[]="Set month";
const char SetMonth_TW[]="�]�m���";
const char SetMonth_JC[] = "�����·�" ;
const char SetMonth_JP[] ="���̐ݒ�";
const char * SetYear_Str[] = { SetMonth_EN,SetMonth_TW,SetMonth_JC,SetMonth_JP };

const char SetMonthUnit_EN[] ="month";
const char SetMonthUnit_TW[] ="��";
const char SetMonthUnit_JC[] ="��" ;
const char SetMonthUnit_JP[] ="��";
const char * SetMonthUnit_Str[] = { SetMonthUnit_EN,SetMonthUnit_TW,SetMonthUnit_JC,SetMonthUnit_JP };
	
const OLED_PositionStruct ComfirmationPositions[LanguageNum][2] =
{
{20,45,76,45},
{36,45,84,45},
{36,45,84,45},
{8,45,84,45}
};




