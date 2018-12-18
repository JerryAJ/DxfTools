#include "stdafx.h"
#include "GameManager.h"
#include "utils.h"
#include "TaskManager.h"
#include "baseaddr.h"
#include "GameCall.h"
#include "Import.h"
#include "Dr7ApiHook.h"
#include "GetDirection.h"

bool g_bInitGame = false;
DWORD g_nCrossMapTimes = { 0 };
bool g_bEnterMap = false;
bool g_bEnterRole = false;
bool g_bEnterSelectRole = false;
bool g_bKillBoos = false;
bool g_bRecvGameRewords = false;

DWORD g_LastTimeRecord = 0;
DWORD g_CurrTimeRecord = 0;
DWORD g_CurrentRoleIndex = 0;

PVOID g_HookQuanPing = { 0 };
PVOID g_HookRecv = { 0 };

/*
// ��ι���
*/
char szData[0x1000] = { 0 };
DWORD JumpStruct[0x10] = { 0 };
DWORD JmpSzData = (DWORD)szData;
DWORD JmpData = (DWORD)szData + 2 * 20;

typedef union _FLOAT_DWORD
{
	DWORD dwData;
	float fData;
}FLOAT_DWORD;


std::unique_ptr<GameManager> GameManager::m_pInstance(new GameManager());
GameManager::GameManager() : m_bExitMainThread(false)
{
}


GameManager::~GameManager()
{
	if (m_hMainThread) {
		m_bExitMainThread = true;
		//WaitForSingleObject(m_hMainThread, INFINITE);
		//CloseHandle(m_hMainThread);
	}
}

GameManager * GameManager::GetInstance()
{
	return m_pInstance.get();
}

bool GameManager::Init()
{	
	//������
	//DWORD hTenRPCS = (DWORD)GetModuleHandle(L"TenRPCS.dll");
	//DWORD hGameRpcs = (DWORD)GetModuleHandle(L"GameRPCS.dll");
	//DWORD hDnfBase = (DWORD)GetModuleHandle(L"DNFBase.dll");
	//ScanTenRPCS(NULL);
	{
		//����

		//�����ȫ������
		//GameCall::SwitchEPCallerNew((PVOID)(Base_HookGouziAddr - 5), HookQuanPing, NULL);
		GameCall::SwitchEPCallerNew((PVOID)(Base_HookRecvAddr), HookRecv, NULL);

		////�������
		//GameCall::SwitchEPCaller(PVOID(Base_SendPacketAddr + 0x8), NULL, NULL);

		//HOOK ��� 026EA4E0    E8 F8961D0E     CALL TenRPCS.108C3BDD
		//GameCall::SwitchEPCallerNew((PVOID)(0x026EA4E0), HookDuoDuan, NULL);


		{
			//������CD

			///*GameRpcs.dll+1B0300 - C3                    - ret */
			//utils::WriteValue<BYTE>(hGameRpcs + 0x1B0300, 0xC3);
	
			///*GameRpcs.dll + 4EC529 - 83 C4 20 - add esp, 20 { 32 }
			//GameRpcs.dll + 4EC52C - E9 84A9A9F2 - jmp dnf.exe + 2026EB5*/
			//char szNotCDHook[8] = { 0x83 ,0xC4 ,0x20, 0xE9 , 0x0, 0x0 , 0x0 , 0x0 };
			//DWORD CallValue = (DWORD)0x02426EB5 - (hGameRpcs + 0x4EC52C) - 5;
			//*(DWORD*)(&szNotCDHook[4]) = CallValue;
			//utils::WriteBytes(hGameRpcs + 0x4EC529, szNotCDHook, 8);
		}

		{
			////�����ζ���
			//FLOAT_DWORD hurt;
			//hurt.fData = 1138888.0;
			//for (auto i = 0; i < 2; i++)
			//{
			//	*(DWORD*)(szData + i * 20) = 0;
			//	*(DWORD*)(szData + i * 20 + 4) = 0;
			//	*(DWORD*)(szData + i * 20 + 8) = hurt.dwData;
			//	*(DWORD*)(szData + i * 20 + 12) = 1;
			//	*(DWORD*)(szData + i * 20 + 16) = 1;
			//}
		}
		
		{
			//
			//GameRpcs.dll+49377A - C3                    - ret 
			//utils::WriteValue<BYTE>(hGameRpcs + 0x49377A, 0xC3);
			//�����1
			//GameRpcs.dll+48C0F1 - C3                    - ret 
			//utils::WriteValue<BYTE>(hGameRpcs + 0x48C0F1, 0xC3);
		}

		// �˺���� û����
		//026D3A1D    E8 CBDF5D0F     CALL TenRPCS.11CB19ED 
		//GameCall::SwitchEPCaller(PVOID(0x026D3A1D), NULL, NULL);

		//026D35B0    E8 28D61457     CALL TenRPCS.59820BDD                    ; //�˺����
		//GameCall::SwitchEPCaller(PVOID(0x026D35B0), NULL, NULL);

		/*
		��ͼ���
		// 028E6D76    E8 A6B4EE0F     CALL TenRPCS.127D2221
		*/
		//GameCall::SwitchEPCaller(PVOID(0x028E6D76), NULL, NULL);

		// ���Ӽ��
		// 027F84B4    E8 6ABCE457     CALL TenRPCS.5A644123
		//GameCall::SwitchEPCaller(PVOID(0x027F84B4), NULL, NULL);
		// 0284AED6    E8 8282DA0F     CALL TenRPCS.125F315D
		//GameCall::SwitchEPCaller(PVOID(0x0284AED6), NULL, NULL);

		//BOOS�����˺���� û����
		// 028010DD    E8 250DB056     CALL TenRPCS.59301E07
		//GameCall::SwitchEPCaller(PVOID(0x028010DD), NULL, NULL);

	}


	{
		//HOOK �Զ��屶��
		/*char szHook[6] = { 0xE9 , 0x0, 0x0 , 0x0 , 0x0 , 0x90 };
		DWORD CallValue = (DWORD)HookBiegongProxy - Base_HookBeiGongAddr - 5;
		*(DWORD*)(&szHook[1]) = CallValue;*/
		//utils::WriteBytes(Base_HookBeiGongAddr, szHook, 6);
	}

	////HOOK ����
	//{
	//	char szNopData[] = { 0x90,0x90 ,0x90 ,0x90,0x90 ,0x90,0x90,0x90 ,0x90,0x90,0x90 ,0x90 };
	//	//utils::WriteBytes((Base_HookBaiziAddr + 7), szNopData, 6);
	//}
	
	//CLIENT_ID client_id;
	//SIZE_T stack_reserve = 0, stack_commit = 0;
	//auto state = RtlCreateUserThread(GetCurrentProcess(), NULL, FALSE, NULL,
	//	stack_reserve, stack_commit,
	//	MainThreadProc, NULL, &m_hMainThread, &client_id);
	//HideThread(m_hMainThread);
	//if(m_hMainThread)
	//	CloseHandle(m_hMainThread);
	return true;
}



void __stdcall DelRecvData(DWORD ptr)
{
	 DWORD Id = utils::ReadValue<WORD>(ptr + 1);
	 DWORD Len = utils::ReadValue<DWORD>(ptr + 3);

	 switch (Id)
	 {
	 case 573:
	 {
		 //ѡ���ɫ����
		 g_bEnterSelectRole = true;
		 g_bEnterRole = false;
		 break;
	 }
	 case 1283: //188Ҳ��
	 {
		 //�����ɫ������
		 g_bEnterRole = true;

		 //auto player = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);
		 //if (player) {
			// //����ƫ��
			// auto cloth = utils::ReadValue<DWORD>(player + 0x3254);
			// if (cloth) {
			//	 //GameCall::Encrypt(cloth + 0x7F8, 100000); //gongji
			//	 GameCall::Encrypt(cloth + 0x684, 100000); //liliang

			// }
		 //}
		 break;
	 }
	 case 1098:
	 {
		 //��ͼ����
		 g_bEnterMap = true;

		 //auto player = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);
		 //if (player) {
			// //����ƫ��
			// auto cloth = utils::ReadValue<DWORD>(player + 0x3254);
			// if (cloth) {
			//	 //GameCall::Encrypt(cloth + 0x7F8, 10000); //gongji
			//	 GameCall::Encrypt(cloth + 0x684, 10000); //liliang

			// }
		 //}
		 break;
	 }
	 case 24:
	 {
		 //�����л�
		 break;
	 }
	 case 27:
	 {
		 //ѡͼ����
		 break;
	 }
	 case 28:
	 {
		 //ѡͼȷ��
		 break;
	 }
	 case 29:
	 {	
		 //����С��ͼ����

		 DWORD MonserNum = utils::ReadValue<BYTE>(ptr + 16 + 18);
		 for (DWORD i = 0; i < MonserNum; i++)
		 {
			 //std::wstringstream type;
			 //type << L"��������Ϊ:" << utils::ReadValue<BYTE>(ptr + 15 + 33 + 21 * i) << L" �ȼ�Ϊ��"<< utils::ReadValue<BYTE>(ptr + 15 + 32 + 21 * i);
			 //GameCall::Bulletin(type.str());

			// utils::WriteValue<DWORD>(ptr + 15 + 33 + 21 * i, 5); 
			// utils::WriteValue<DWORD>(ptr + 15 + 28 + 21 * i, 3);
			// utils::WriteValue<BYTE>(ptr + 15 + 32 + 21 * i, 1);
		 }

		 DWORD temp = utils::ReadValue<BYTE>(ptr + 34);
		 DWORD tempobj = (temp - 1) * 21;
		 DWORD JZNum = utils::ReadValue<BYTE>(ptr + 56 + tempobj);

		 if (JZNum >= 20) {
			 JZNum = 0;
		 }

		 for (DWORD i = 0; i < JZNum; i++)
		 {
			 DWORD Id = utils::ReadValue<DWORD>(ptr + tempobj + 58 + 21 * i);
			 DWORD player = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);

			GameCall::PickUpItem(Id, GameCall::GetPos(player ,0), GameCall::GetPos(player, 1));
		 }

		 break;
	 }
	 case 38:
	 {
		 //��������
		 if (Len != 27)
		 {
			 DWORD ItemNum = utils::ReadValue<BYTE>(ptr + 18);
			 if (ItemNum > 10)
			 {
				 ItemNum = 0;
			 }

			 for (DWORD i = 0; i < ItemNum; i++)
			 {
				 DWORD Id = utils::ReadValue<DWORD>(ptr + 19) + i;
				 DWORD player = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);

				 GameCall::PickUpItem(Id, GameCall::GetPos(player, 0), GameCall::GetPos(player, 1));
			 }

		 }
		 break;
	 }
	 case 30:
	 {
		 //����ȷ��
		 break;
	 }
	 case 31:
	 {
		 //��������
		// GameCall::SetScore(5201314);
		 break;
	 }
	 case 34:
	 {
		 //��������
		/* g_bKillBoos = true;
		 g_nCrossMapTimes++;
		 std::wstringstream CrossTimes;
		 CrossTimes << L"�Զ�ˢͼ [" << g_nCrossMapTimes << L"] �� ,��ǰƣ��Ϊ [" << GameCall::GetCurrentPL() << L"/" << GameCall::GetMaxPL() << L"]";
		 GameCall::Bulletin(CrossTimes.str());*/
		 //GameCall::AntiFlop(0, 1);
		 break;

	 }
	 case 261:
	 {
		 //�յ����ƽ���
		/* g_bRecvGameRewords = true;
		 GameCall::LeaveMap();
		 DelAllPacketItem();*/
		 break;
	 }
	 case 199:
	 {
		 //�����ж�
		 //GameCall::Bulletin(L"���������ж�");
		 break;
	 }
	 case 2:
	 case 3:
	 {		
	/*	 if (GameCall::GetGameState() == 3)
		 {
			 static bool Bfis = false;
			 if (GameCall::BOpenDoor())
			 {
				 GetDirection d;
				 auto map = d.GetData(false);
				 if (map.CastPL > 0)
				 {
					 int next = d.GetNextDirection(map.path[0], map.path[1]);
					 if (Bfis)
					 {
						 SearchDoorPosAndMove(next, true);
						 Bfis = false;
					 }
					 else {
						 SearchDoorPosAndMove(next, false);
						 Bfis = true;
					 }

				 }
			 }
			 else {
				 GetItemFromGround();
			 }
		 }
		 */

		 break;
	 }

	 default:
		 break;
	 }


	
}



void  _declspec(naked) HookRecv()
{
	__asm
	{
		//����1
	/*	CALL g_HookRecv;

		PUSH EDI;
		MOV EDI, DWORD PTR SS : [EBP + 14h];
		PUSH EDI;
		CALL DelRecvData;
		POP EDI;

		RETN;*/

		//����2
		/*PUSH EBP
		MOV EBP, ESP
		CALL g_HookRecv;
		MOV ESP,EBP
		POP EBP*/


		MOV ESP, EBP;
		ADD ESP, 0x10;

		POPFD;
		POPAD;

		PUSH EDI;
		CALL DelRecvData;

		PUSH EBP;
		MOV EBP, ESP;
		PUSH EBX;
		MOV EBX, [EBP + 0Ch];
		PUSH Base_HookRecvAddr+ 0x7;
		RETN;
	}
}

void _declspec(naked) HookDuoDuan()
{
	__asm
	{
		MOV ESP, EBP;
		ADD ESP, 0x10;

		POPFD;
		POPAD;


		//utils::WriteValue<DWORD>((DWORD)JumpStruct + 4, (DWORD)szData);
		//utils::WriteValue<DWORD>((DWORD)JumpStruct + 8, (DWORD)szData + 2 * 20);
		//utils::WriteValue<DWORD>((DWORD)JumpStruct + 12, (DWORD)szData + 2 * 20);

		PUSH EBP
		MOV EBP, ESP

		PUSH EAX;
		PUSH EDX;

		MOV EAX, DWORD PTR SS : [EBP + 0x14];
		MOV EDX, JmpSzData;
		MOV[EAX + 4], EDX;
		MOV EDX, JmpData;
		MOV[EAX + 8], EDX;
		MOV[EAX + 12], EDX;
		MOV  DWORD PTR SS : [EBP + 0x14], EAX;
		POP EDX;
		POP EAX;

		PUSH - 1
		PUSH 0x26EA4E5
		RETN
	}
}

void _declspec(naked) HookFenLie()
{
	__asm
	{
		MOV ESP, EBP;
		ADD ESP, 0x10;

		POPFD;
		POPAD;

		PUSH EBP;
		MOV EBP, ESP;
		PUSH - 1;
		MOV DWORD PTR SS : [EBP + 10h], 20;
		PUSH 26D35B5h;
		RETN;
	}
}

//
//void  _declspec(naked) HookDecHealth()
//{
//	__asm
//	{
//
//		MOV ESP, EBP;
//		ADD ESP, 0x10;
//
//		POPFD;
//		POPAD;
//
//		PUSH EBP;
//		MOV EBP, ESP;
//
//		MOV EAX, [ECX + 824h];
//		CMP EAX, 0;
//		JZ label;
//		MOV EAX, DWORD PTR SS : [EBP + 08h];
//		CMP EAX, 288888;
//		JLE label3;
//		SUB EAX, 200000;
//		MOV DWORD PTR SS : [EBP + 08h], EAX;
//		JMP label2;
//	label3:
//		MOV DWORD PTR SS : [EBP + 08h], 0;
//		JMP label2;
//	label:
//		MOV DWORD PTR SS : [EBP + 08h],999;
//	label2:
//		SUB ESP, 0C0h;
//		PUSH 26B5459h;
//		RETN;
//	}
//}



void _declspec(naked) HookBiegongProxy()
{
	__asm
	{
		//��ȡ����
		MOV EAX, DWORD PTR SS : [EBP + 0x14];
		CMP EAX, 0;
		JZ label_retn;
		MOV EAX, [EAX + Base_CampOffsetAddr];
		MOV  DWORD PTR SS : [EBP + 0x10], 0x0;
		CMP EAX, 0;
		JNZ label_retn;
		////��������
		MOV  DWORD PTR SS : [EBP + 0x10], 0x30000;
	label_retn:
		PUSH Base_HookBeiGongAddr + 6;
		RETN;
	}
}




DWORD g_LastIndex = 0;
DWORD g_LastEax = 0;
void  _declspec(naked) HookQuanPing()
{
	
	__asm
	{
		MOV ESP, EBP;
		ADD ESP, 0x10;

		POPFD;
		POPAD;

		//����ԭʼ��EAX
		MOV g_LastEax, EAX;

		//�жϹ��������Ƿ�����ң����������Ҷ�������ת��������
		MOV	 EAX, [ESI + Base_CampOffsetAddr];
		CMP  EAX, 0x0;
		JNZ  label_not_attack;

	//	//�жϱ����������Ƿ��ǹ������ǹ���򹥻�
	//	/*MOV  EAX, [EDI + Base_CampOffsetAddr];
	//	CMP  EAX, 0x64;
	//	JNZ   label_not_attack;*/

	//	//������
		MOV  EAX, [EDI + Base_TypeOffsetAddr];
		//������ֱ�ӹ���
		CMP  EAX, 0x421;
		JZ   label_attack;
		CMP  EAX, 0x7740;
		JZ   label_attack;
		CMP  EAX, 0x1021;
		JZ   label_attack; 
		CMP  EAX, 0x221;
		JZ   label_attack;
		CMP  EAX, 0x21; //������
		JZ   label_attack;

		//����
		CMP  EAX, 0x211;
		JZ   label_jugde_health;
		CMP  EAX, 0x111;
		JNZ  label_not_attack;

	label_jugde_health:
		//�жϹ����Ƿ������������������ת��������
		MOV  EAX, [EDI + Base_MonsHealOffsetAddr];
		CMP  EAX, 0;
		JLE   label_not_attack;

	label_attack:
		MOV AL, 1;
		MOV BYTE PTR SS:[EBP + 17h], 1;
		TEST AL, AL;
		PUSH Base_HookGouziAddr;
		RETN;

		//ԭʼ��������
	label_original_attack:
		MOV  EAX, g_LastEax;
		MOV BYTE PTR SS : [EBP + 17h], AL;
		TEST AL, AL;
		PUSH Base_HookGouziAddr;
		RETN;

		//��������
	label_not_attack:
		MOV EAX, 0;
		MOV BYTE PTR SS : [EBP + 17h], 0;
		TEST AL, AL;
		PUSH Base_HookGouziAddr;
		RETN;
	}

}



DWORD GameManager::MainThreadProc(LPVOID lpParameter)
{
	Sleep(6000);

	//��ǰ�߳�id
	//GameRPCS.DLL + 3F4288
	//utils::WriteValue<DWORD>((DWORD)GetModuleHandle(L"GameRPCS.DLL") + 0x3F4288, GetCurrentThreadId());

	DWORD RoleIndex = 0;
	while (!GameManager::GetInstance()->BExitGame())
	{
		switch (GameCall::GetGameState())
		{
		case 0:
		{
			Sleep(5000);

			//ѡ��ɫ����
			if (g_bEnterSelectRole)
			{

				auto count = GameCall::GetAllRoleCount();
				if (count > 0) {
					if (g_CurrentRoleIndex < count)
					{
						//GameCall::Bulletin(L"�����ɫ");
						Sleep(1500);
						GameCall::EnterRoleByIndex(g_CurrentRoleIndex);
						Sleep(2000);
					}
					else {
						std::wstringstream ss;
						ss << L"��ˢ�� [" << g_CurrentRoleIndex << L"] ����ɫ�����н�ɫ�����";
						GameCall::Bulletin(ss.str());
						GameCall::RandomSleep();
						return 0;
					}
						
				}
				else {
					GameCall::Bulletin(L"��ǰ�˺�û�н�ɫ");
					GameCall::RandomSleep();
					return 0;
				}
				g_bEnterSelectRole = false;
			}
			break;
		}
		case 2:
		{
			break;
		}
		case 1:
		{
			GameCall::RandomSleep(200);
			if (g_bEnterRole)
			{
				
				if (GameCall::GetCurrentPL() > 5)
				{
					DWORD level = GameCall::GetPlayerLevel();
					if (level >= 83)
					{
						//�Զ�����
						DWORD MapId = 104;

					/*	if (g_nCrossMapTimes % 2 == 0) {
							MapId = 192;
						}*/

						GameCall::EreaCall(MapId);
						Sleep(300);
						GameCall::SelectMap();
						Sleep(500);
						DWORD Easy = GetMapCurDiff(MapId);
						GameCall::EnterMap(MapId, Easy);
						Sleep(100);
					}
					else
					{
						//�Զ�����
						TaskManager::GetInstance()->AutoTask();
					}
				}
				else
				{
					g_CurrentRoleIndex++;
					//ƣ��ֵΪ0
					GameCall::RandomSleep();
					std::wstringstream text;
					text << L"�� [" << g_CurrentRoleIndex << L"] ����ɫ��ˢ�꣬�����л���һ����ɫ";
					GameCall::Bulletin(text.str());
					GameCall::RandomSleep();
					GameCall::ReturnToSelectRole();
					GameCall::RandomSleep();
				}

			}
			break;
		}
		case 3:
		{
			if (!GameCall::IsPlayerAlive())
			{
				GameCall::LeaveMap();
				GameCall::RandomSleep();
			}

			if (GameCall::IsTask())
			{
				GameCall::MemKeyBorad(326, true);
				Sleep(10);
				break;
			}

			static DWORD time = clock();
			if (GameCall::BOpenDoor())
			{
				GameCall::RandomSleep(300);

				GetDirection d;
				auto map = d.GetData(false);
				if (map.CastPL > 0)
				{
					int next = d.GetNextDirection(map.path[0], map.path[1]);
					if ((DWORD)clock() - time > 4000)
					{
						GameCall::Bulletin(L"ǿ��˳;");
						GameCall::MoveToNextMap(next);
						time = clock();
					}
					else
					{
						Sleep(300);
						SearchDoorPosAndMove(next, false);
						Sleep(300);
						SearchDoorPosAndMove(next, true);	
					}
				}

				GameCall::RandomSleep(100);
			}
			else {
				GetItemFromGround();
				time = clock();
			}

			break;
		}
		default:
			break;
		}
		Sleep(30);
	}
	return 0;
}

bool GameManager::BExitGame()
{
	return m_bExitMainThread;
}


void DelAllPacketItem()
{
	DWORD packetbase = utils::ReadValue<DWORD>(Base_PacketTravseAddr);
	if (!packetbase) {
		return;
	}

	DWORD base= utils::ReadValue<DWORD>(packetbase + 0x58);
	if (!base) {
		return;
	}

	DWORD Index = 0;
	for (int i = 0; i < 56; i++)
	{
		DWORD packetitem = utils::ReadValue<DWORD>(base + 0x24 + i * 4);
		if (!packetitem) {
			continue;
		}

		DWORD quality = utils::ReadValue<DWORD>(packetitem + 0x178);
	
		if (quality < 3) {
			if (GameCall::GetPlayerLevel() < 40)
			{
				GameCall::BreakDownEquipment(i + 9);
			}
			else {
				GameCall::BreakDownEquipment(i + 9);
				//GameCall::SellEquipment(i + 9);
			}
			//GameCall::BreakDownCall(i + 9);
			
			Index++;
		}

		if (Index > 4) {
			break;
		}
	}
	/*if(Index > 0)
		GameCall::ClearUpPacket();*/
	//if (Index > 0) {
	//	std::wstringstream text;
	//	text << L"�ֽ�װ�� [" << Index << L"] ��";
	//	GameCall::Bulletin(text.str());
	//}
}

void GetItemFromGround()
{

	//1. �ж���Ҷ����Ƿ����Ŷ
	DWORD playerObj = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);
	if (playerObj)
	{
		//utils::LogText("HXL: playerObj=[%p]", playerObj);
		DWORD Town = utils::ReadValue<DWORD>(playerObj + Base_MapOffsetAddr);
		DWORD StartAddr = utils::ReadValue<DWORD>(Town + Base_MapStartAddr);
		DWORD EndAddr = utils::ReadValue<DWORD>(Town + Base_MapStartAddr + 0x4);

		DWORD index = 0;
		DWORD skIndex = 0;
		for (auto i = StartAddr; i < EndAddr; i += 4)
		{
			DWORD obj = utils::ReadValue<DWORD>(i);
			if (!obj || obj == playerObj) {
				continue;
			}

			//��ȡ��������
			DWORD camp = utils::ReadValue<DWORD>(obj + Base_CampOffsetAddr);
			DWORD type = utils::ReadValue<DWORD>(obj + Base_TypeOffsetAddr);
			
			if (camp > 0)
			{
				if (type == 529 /*|| type == 545*/ || type == 273)
				{
					DWORD health = utils::ReadValue<DWORD>(obj + Base_MonsHealOffsetAddr);
					if (health > 0)
					{

						//GameCall::SetPos(obj, GameCall::GetPos(playerObj, 0), GameCall::GetPos(playerObj, 1), 0);
						//////GameCall::SetSkillHurt(70023, 399999);
						////GameCall::SkillCall(playerObj, GameCall::GetPos(obj, 0), GameCall::GetPos(obj, 1), 0, 70023, 0);
						////Sleep(20);
						//skIndex++;

						//GameCall::MonsDecHealthCall(playerObj, obj, 0);
						//GameCall::MemKeyBorad(314,false);
						while (utils::ReadValue<DWORD>(obj + Base_MonsHealOffsetAddr))
						{
							GameCall::MemAttack();
							Sleep(10);
						}
						
					}
					else {
						GameCall::SetPos(playerObj, GameCall::GetPos(obj, 0), GameCall::GetPos(obj, 1), 0);
						GameCall::RandomSleep(300);
					}
				}
			}
		}

	}
}

int GetGoodItemFromGround(bool bMove)
{

	//1. �ж���Ҷ����Ƿ����Ŷ
	DWORD playerObj = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);
	if (playerObj)
	{
		//utils::LogText("HXL: playerObj=[%p]", playerObj);
		DWORD Town = utils::ReadValue<DWORD>(playerObj + Base_MapOffsetAddr);
		DWORD StartAddr = utils::ReadValue<DWORD>(Town + Base_MapStartAddr);
		DWORD EndAddr = utils::ReadValue<DWORD>(Town + Base_MapStartAddr + 0x4);

		DWORD skIndex = 0;
		for (auto i = StartAddr; i < EndAddr; i += 4)
		{
			DWORD obj = utils::ReadValue<DWORD>(i);
			if (!obj || obj == playerObj) {
				continue;
			}

			//��ȡ��������
			DWORD camp = utils::ReadValue<DWORD>(obj + Base_CampOffsetAddr);
			DWORD type = utils::ReadValue<DWORD>(obj + Base_TypeOffsetAddr);
			//utils::LogText("HXL: camp=%d type=%d ", camp, type);
			if (camp == 200)
			{

				if (type == 289)
				{
					DWORD itemId = GameCall::Dectypt(obj + 172);
					GameCall::PickUpItem(itemId, GameCall::GetPos(playerObj, 0), GameCall::GetPos(playerObj, 1));
			/*		if (bMove)
					{
						GameCall::SetPos(obj, GameCall::GetPos(playerObj, 0), GameCall::GetPos(playerObj, 1), 0);
						Sleep(100);
						GameCall::MemAttack();
					}*/
					skIndex++;
				}
			}

		}

		if (skIndex > 0)
		{
			return skIndex;
		}

	}

	return 0;
}

void SearchDoorPosAndMove(int next, bool bFis)
{
	DWORD player = utils::ReadValue<DWORD>(Base_PlayerObjectAddr);
	if (!player) {
		return;
	}

	DWORD RoomId = utils::ReadValue<DWORD>(Base_NoticeCallECXAddr - 0x8);
	if (!RoomId) {
		return;
	}

	DWORD TimeTick = utils::ReadValue<DWORD>(RoomId + Base_TimeTickAddr);
	if (!TimeTick) {
		return;
	}

	DWORD PntMoveToPos = utils::ReadValue<DWORD>(TimeTick + Base_PntMoveToPosOffset);
	if (!PntMoveToPos) {
		return;
	}

	DWORD PosStruct = PntMoveToPos + next* 0x28  + Base_PntMoveToPosOffsetAddr;
	if (!PosStruct) {
		return;
	}

	DWORD XPos = utils::ReadValue<DWORD>(PosStruct + 0);
	DWORD YPos = utils::ReadValue<DWORD>(PosStruct + 4);
	DWORD XPosArg = utils::ReadValue<DWORD>(PosStruct + 8);
	DWORD YPosArg = utils::ReadValue<DWORD>(PosStruct + 12);

	DWORD RealXPos = { 0 };
	DWORD RealYPos = { 0 };
	if (next == 0)
	{
		RealXPos = XPos + XPosArg + 20;
		RealYPos = YPos + YPosArg / 2;
	}
	else if (next == 1)
	{
		RealXPos = XPos  - 20;
		RealYPos = YPos + YPosArg / 2;
	}
	else if (next == 2)
	{
		RealXPos = XPos + XPosArg / 2;
		RealYPos = YPos + YPosArg + 20;
	}
	else {
		RealXPos = XPos + XPosArg / 2;
		RealYPos = YPos - 20;
	}

	if (bFis) {
		GameCall::SetPos(player, RealXPos, RealYPos, 0);
	}
	else {
		GameCall::SetPos(player, XPos + XPosArg / 2, YPos, 0);
	}
	
}


ULONG NTAPI ScanTenRPCS(PVOID Parameter)
{
	UNREFERENCED_PARAMETER(Parameter);

	NTSTATUS Status = 0;
	PRDE_LDR_DATA_TABLE_ENTRY SectionHandle = NULL;
	DWORD Begin = NULL;
	BYTE Pattern[] = "\x8d\x64\x24\x04\x60\x9c\x68";

	Status = LdrFindEntryForAddress(GetModuleHandle(L"TenRPCS.dll"),(PLDR_DATA_TABLE_ENTRY *)(&SectionHandle));

	if (NT_SUCCESS(Status)) {
		Begin = (DWORD)SectionHandle->DllBase;

		utils::LogText(
			"HXL: Begin [%p] End [%p]\n",
			Begin,
			Begin + SectionHandle->SizeOfImage);

		DWORD end = (DWORD)SectionHandle->DllBase + SectionHandle->SizeOfImage;
		while (TRUE)
		{
			DWORD Result = utils::FindPattern(Begin, end,Pattern,"*******");

			////utils::LogText(
			////	"HXL: Result [0x%08x] Begin [%p] End [%p]\n",
			////	Result,
			////	Begin);


			if (Result == 0)
			{
				break;
			}

			utils::LogText("HXL: SwitchEP[%p] Begin [%p] End [%p]\n",Result,Begin);
			//GameCall::SwitchEP((PVOID)Result, NULL , NULL);

			Begin = Result + 8;
			//utils::LogText("HXL: Result [%p] Begin [%p] \n", Result, Begin);
		}
	}

	return Status;
}


DWORD GetMapCurDiff(DWORD MapId)
{

	DWORD BaseRole = utils::ReadValue<DWORD>(Base_RoleObjectAddr);
	if (!BaseRole) {
		return 0;
	}

	DWORD MapOffset = utils::ReadValue<DWORD>(BaseRole + Base_MapTravseOffsetAddr);
	if (!MapOffset) {
		return 0;
	}

	DWORD startaddr = utils::ReadValue<DWORD>(MapOffset + Base_MapTravseStartAddr);
	DWORD endaddr = utils::ReadValue<DWORD>(MapOffset + Base_MapTravseStartAddr + 4);

	for (DWORD object = startaddr; object < endaddr; object += Base_MapTravseObjectSize)
	{

		DWORD Id = utils::ReadValue<DWORD>(object);
		if (MapId == Id) {
			return utils::ReadValue<DWORD>(object + 0xc);
		}
	}
	return 0;
}

bool HideThread(HANDLE hThread)
{
	typedef NTSTATUS(NTAPI *pNtSetInformationThread)(HANDLE, UINT, PVOID, ULONG);
	NTSTATUS Status;

	pNtSetInformationThread NtSIT = (pNtSetInformationThread)GetProcAddress(GetModuleHandle(TEXT("ntdll.dll")), "NtSetInformationThread");

	if (NtSIT == NULL)
	{
		return false;
	}

	if (hThread == NULL)
		Status = NtSIT(GetCurrentThread(),
			0x11,
			0, 0);
	else
		Status = NtSIT(hThread, 0x11, 0, 0);

	if (Status != 0x00000000)
		return false;
	else
		return true;
}
//DWORD GetMapCurDiff(DWORD MapId)
//{
//	DWORD Count = 0;
//
//	DWORD OFFSET = 0x70;//������� 
//
//	ULONG_PTR BaseAddress = READOFFSET(��ɫ��ַ, ƫ��_��ͼ��Ϣ, 0x3f0 + 0);
//
//	Count = (READOFFSET(��ɫ��ַ, ƫ��_��ͼ��Ϣ, 0x3f0 + 4) - BaseAddress) / OFFSET;
//
//	while (Count--)
//	{
//		if (MapId == (DWORD)������(BaseAddress + Count * OFFSET))
//		{
//			return (UCHAR)������(BaseAddress + Count * OFFSET + 0xc);//c�ǵ�ͼ����Ѷ�
//		}
//	}
//	return 0;
//}
