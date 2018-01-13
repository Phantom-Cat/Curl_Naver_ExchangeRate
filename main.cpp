#include "stdafx.h"
#include <curl/curl.h>
#include <string.h>
#include <iostream>
#include <conio.h>
#define MAX_STR_LEN 4000

//구조체
typedef struct _money{
	char type[20]; //외화 종류
	double value; //가격
} money;

// 문자열 우측 공백문자 삭제 함수
char* rtrim(char* s) {
	char t[MAX_STR_LEN];
	char *end;
	strcpy_s(t, s);
	end = t + strlen(t) - 1;
	while (end != t && isspace(*end))
		end--;
	*(end + 1) = '\0';
	s = t;

	return s;
}

// 문자열 좌측 공백문자 삭제 함수
char* ltrim(char *s) {
	char* begin;
	begin = s;

	while (*begin != '\0') {
		if (isspace(*begin))
			begin++;
		else {
			s = begin;
			break;
		}
	}
	return s;
}

// 문자열 앞뒤 공백 모두 삭제 함수
char* trim(char *s) {
	return rtrim(ltrim(s));
}

// double_hunter에서 만든 실수에 있는 ','제거
double final_hunter(char *hunted)
{
	if (strlen(hunted)>6){
		for (int i = 1; i < strlen(hunted) - 1; i++){ //1000넘어가는 숫자에서 문자열에 있는 , 삭제
			hunted[i] = hunted[i + 1];
		}
		hunted[strlen(hunted) - 1] = NULL;
	}
	return atof(hunted); // 문자열 -> 실수 변환
}

// 파싱한 문자열에서 실수만 긁어오기
double double_hunter(char *p_value){
	int len = strlen(p_value) - 5 - 17;
	char hunted[10];
	for (int i = 0; i < len; i++){
		hunted[i] = p_value[i + 17];
	}
	hunted[len] = NULL;
	return final_hunter(hunted); //실수 리턴
}

// CURL PARSER
void parser(){
	CURL *curl_parser;
	CURLcode res;

	FILE* file;
	file = fopen("parsed.txt", "w");

	curl_parser = curl_easy_init();

	if (curl_parser) {
		curl_easy_setopt(curl_parser, CURLOPT_URL, "http://info.finance.naver.com/marketindex/exchangeList.nhn");	//접속할 주소
		curl_easy_setopt(curl_parser, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl_parser, CURLOPT_WRITEDATA, file);
		res = curl_easy_perform(curl_parser);

		curl_easy_cleanup(curl_parser);
	}

	fclose(file);
}

// DATA Control
void data_control(money list[]){
	char s[101] = "";
	char *p_type = NULL;
	char *p_value = NULL;
	int i = 0, j = 0;
	FILE* file;

	file = fopen("parsed.txt", "r");  // 파일 열기
	
	while (!feof(file))  // 파일의 끝이 아닐동안
	{
		p_type = NULL;
		p_value = NULL;
		fgets(s, 100, file); // 최대 100바이트까지 읽어오기
		p_type = strstr(s, "					");
		p_value = strstr(s, "<td class=\"sale\">");
		if (p_type != NULL) {
			if (p_type[5] != '\n') { // 공백 거름
				strcpy(list[i].type, trim(p_type)); // 앞뒤 공백 걸러서 구조체 저장
				p_type = NULL;
				++i;
			}
		}
		if (p_value != NULL) {
			list[j].value = double_hunter(trim(p_value)); // 파싱된 문자열 앞뒤 공백 거르고 실수 추출해서 저장
			p_value = NULL;
			++j;
		}
	}
	
	fclose(file);

}

int main() {
	int sw, type = 0;
	int size = 44;
	double value = 0; // 가격
	money list[44];

	parser(); // 파싱함수
	data_control(list); // 데이터 저장
	
	while (1) {
		sw = 0;
		value = 0;
		type = 0;
		system("cls");
		printf("=============================\n");
		printf("실시간 환율 계산기 입니다\n");
		printf("원하시는 메뉴를 선택하세요\n");
		printf("=============================\n");
		printf("1.외화 -> 원화\n");
		printf("2.원화 -> 외화\n");
		printf("3.환율 정보 출력\n");
		printf("4.환율 정보 업데이트\n");
		printf("5.종료\n");
		printf("=============================\n");
		scanf("%d", &sw);
		
		switch (sw) {
		
		case 1:
			system("cls");
			printf("=======================================\n");
			printf("원하시는 외화 번호를 선택하여 주세요\n");
			printf("=======================================\n");
			for (int i = 0; i < size; i++){
				printf("%d.%s\n", i, list[i].type);
			}
			printf("=======================================\n");
			scanf("%d", &type);
			system("cls");
			if (type != 2 && type != 29 && type != 39) { // if문 : 외화의 환율 기준이 100 : 1 인 외화(ex:100엔에 1000원 하듯이) 검출
				printf("=====================================================\n");
				printf("선택된 외화 종류는 %s입니다\n", list[type].type);
				printf("1 %s = %.2f\n", list[type].type, list[type].value);
				printf("=====================================================\n");
				printf("금액을 입력하세요 : ");
				scanf("%lf", &value);
			}
			else {
				printf("=====================================================\n");
				printf("선택된 외화 종류는 %s입니다\n", list[type].type);
				printf("100 %s = %.2f\n", list[type].type, list[type].value);
				printf("=====================================================\n");
				printf("금액을 입력하세요 : ");
				scanf("%lf", &value);
			}
			system("cls");
			if (type != 2 && type != 29 && type != 39) { // 위 설명과 같음
				printf("=====================================================\n");
				printf("%.2lf %s 는 원화로 %.2lf\\ 입니다.\n", value, list[type].type, list[type].value*value);
				printf("=====================================================\n");
			}
			else {
				printf("=====================================================\n");
				printf("%.2lf %s 는 원화로 %.2lf\\ 입니다.\n", value, list[type].type, (list[type].value*value) / 100);
				printf("=====================================================\n");
			}
			getch();
			break;
		
		case 2:
			system("cls");
			printf("=======================================\n");
			printf("원하시는 외화 번호를 선택하여 주세요\n");
			printf("=======================================\n");
			for (int i = 0; i < size; i++){
				printf("%d.%s\n", i, list[i].type);
			}
			printf("=======================================\n");
			scanf("%d", &type);
			system("cls");
			if (type != 2 && type != 29 && type != 39) {
				printf("=====================================================\n");
				printf("선택된 외화 종류는 %s입니다\n", list[type].type);
				printf("1 %s = %.2f\n", list[type].type, list[type].value);
				printf("=====================================================\n");
				printf("금액을 입력하세요 : ");
				scanf("%lf", &value);
			}
			else {
				printf("=====================================================\n");
				printf("선택된 외화 종류는 %s입니다\n", list[type].type);
				printf("100 %s = %.2f\n", list[type].type, list[type].value);
				printf("=====================================================\n");
				printf("금액을 입력하세요 : ");
				scanf("%lf", &value);
			}
			system("cls");
			if (type != 2 && type != 29 && type != 39) {
				printf("=====================================================\n");
				printf("%.2lf\\ 은 외화로 %.2lf %s 입니다.\n", value, value / list[type].value, list[type].type);
				printf("=====================================================\n");
			}
			else {
				printf("=====================================================\n");
				printf("%.2lf\\ 은 외화로 %.2lf %s 입니다.\n", value, (value / list[type].value) * 100, list[type].type);
				printf("=====================================================\n");
			}
			getch();
			break;
		
		case 3:
			system("cls");
			for (int i = 0; i < size; i++) {
				if (strlen(list[i].type) >= 15)
					printf("%s\t%.2f\n", list[i].type, list[i].value);
				else
					printf("%s\t\t%.2f\n", list[i].type, list[i].value);

			}
			getch();
			break;
		
		case 4:
			parser();
			data_control(list);
			printf("완료!");
			getch();
			break;
		
		case 5:
			return 0;
			break;
		
		default:
			printf("잘못 누르셨습니다...");
			sw = 0;
			getch();
			break;
		}
	}
}