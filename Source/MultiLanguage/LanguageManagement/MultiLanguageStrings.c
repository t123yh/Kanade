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

const OLED_PositionStruct ComfirmationPositions[LanguageNum][2] =
{
{20,45,76,45},
{36,45,84,45},
{36,45,84,45},
{12,45,84,45}
};




