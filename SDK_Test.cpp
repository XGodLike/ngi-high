#include "stdafx.h"
#include "VcyberVoiceSDK.h"
#include <string.h>
#include <curl/curl.h>
#include <json/json.h>
#include "Configs.h"
#include "SdkLib.h"

#include <iostream>

int main(void)
{
	SESSION_HANDLE session;
	char *config = "{\"server_addr\":\"http://10.1.8.110:10028/?v=2.0\",\"app_id\":\"cyw00036\",\"vin\":\"1234444\",\"mac\":\"123123123123\"}";
	CloudVDInit(config);
	printf("初始化...  \n");
	char *str2 = "{\"business\":\"\",\"tts\":1,\"audio_format\":1,\"audio_quality\":10}";
	CloudVDStartSession(str2, &session);
	printf("Session开始...  \n");

	char *audio = "LgDrn09g8sT0X5oAYFrFtBeo9d7NPHT7Y1cHSjT3XpKjdjwfruihtC4Sj9/iUYb/NgDtXaBJOvIpTj1GMXT0ZccaYXypWEAfqOVm3NFV+JW0acgpQmMafz7b8HkLiSGTFwN7bkH1n6tgANKCwz7txaQTGCo91FfS4XBM8j4z5Z0KAsb01S1UubTSFi4PZ6tuZEsRyiNWFjlAmyiDfml1StARhUfcVhaPm3YGRLgSLN3AMPQjd2JxTVZCGv4p4n06dJOauIdu5JA0714A2NgXkJuZPFekyC/6NMEUNbSB9zinPLrZow8TBeUMSOp/fxmsrMhxPBYZTkwRO0jkewP68vBIMGmgt16mJ8f1xDwEwqw73RhgEcctkf4VsYnh67trj9d143dFiyYkH1QAz00aq436fxyqF6gMTlBtwtqVbBVmzOVN0NTM2xxmkjwKDZQG0VVRMqLZZrdt4GKJaLx6nngaz1d0/7xCrDXrZJwyFNoUaNx9kl244FrWbezPd717TADQFrqjPlEm6yCftDnlytvz9BvKvvma1jZdjLHcnVu6RUjQF7kd8LxwiYkgFPT993HwzNhXDkJIahEymhVIAN6rua1XLsGo2q7detpHWQDPP7PBaJM/L8d11wH7yuBTB+E9O3fQFtn/1JIPC9SZLNTSTTkfwsiN2fOirX4RVpq4m1oUz6Ly6y3RVgt1pZt+EOdBOYahOM6H8MyyMU1P/goDmKkPl58JP2QAyjo3GDMm/bmvCSyueuydFD8Uz3UlIzI9J8FKLWEX6jcQG67qXfbb0VOZUmVjPn9lquHamvZALL9sKlWUA4BGCQ1VuzYqmx/JYbGDLq7ZBNFcT3eXra6fjqRVslqxYyrfDTGnvw==";
	CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_FIRST);
	printf("发送导航数据1...  \n");

	audio = "ZwDj0Ea37/cVxXi0rQK6f/QhyyRLwqkzy8+3gKwRXNgigPgEtVyfNUjP8and+OWhtSzbs2gRzcaI2aklLpyqMaw56VZ6piWmYvG8eli8kU09gvRJzHJ4INhjigUn/4SL10kg+24t1nf/SgD1r4nKZ2cnvHsk9dJJSZpxUKWZlyBKnSxt+xzshOdSFLk0s3Z/wKyL8/7zJ8PkzoqjcHbcwI7sN3+q4emjUKV2LmK1gz+vUDwmf1wAx2UepGbC7hepzyBUOmHiJRcezfFhylb0o6YA+fhgF8Eg7jd98Ynnr5+YXo/wWKcoVUwSTVsRLbBOUC8P34z+socBPDd+b17GP9iM0+sZj6qITwZtpTcHdDfAvt88AMu3ZTdXdFc2+Cy7MftO/QCtiylKFmxosDxXx9hYLIr6QjQ06RjkElt6ZgUCm770+j79tifR0hfV35T1f1QAyNvY1YcTr9Vz2RT3uWE4RaR206m3I0mZDP76xZSkSJ1PbsxDpP8zOM5Nly9VNK96SN2dKLnZqEYlVBIPsKmFqlPi9eD1Xrc86hUIR1DnbrX00fKmVQDIPEmBWiqbGYCwTuOG0Z4qd8fGj5yVKWwot2IFccaoB4u9AdPr5K4NVxavQmSYRGaqCTLup+dzCpvebfxVN98EWZspuvVa5qQkiRTRA/CO69RuituPVQDIH8JiTp77xRyu1+LKEeHNyo93uamhzm40O9AfgWx6KOessu5tpyykHMpcIqUvqy3WHrD47RqCdT90x9XwebicHVbxN7f/J6CUEy9m5xdCGQV8PYa/VQDH4KmYhAuXgcrClKIWjaXFsnewbj57VVWGkJP2lJ9L1EPnstQxFFlo3mJSeu1pW5hy/AnVacpdAUQjl2FTb3lFpCfadmqm/dKr8OWKtITmEC+abEQX";
	CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	printf("发送导航数据2...  \n");

	audio = "TADEJpdHkWdd65DdOUZOK+iKG56mPUlR/KowqikyjVMwi1qoOsKnZyT8zhgPRj6kpzmoyHOFooSr2bvYio8RRYhttOnHITiINWZKm3THLgDyragG8UsKEQEyJq9qwPSaloy/SA3raPqXhKoY/17gpLtBCfgybues2nt9pAfPLgDvzWyhIpKh66JhcbEUA/9mGvhrtGAbLALFyruGsrAeJjGG2pxmhL3h+yl9T8u/MgDvMPmeUNQodjdaXaAvUD4IxDSxRKte6vHWn9n3wtyt1RZVMFwgu3A7VQoUISMei4TJQywA7sJZD/cs5X0cPT7OHUoNf0YjkG6BVBO4WjPyWbPsPzFaGTnCvntoIY9sO/80AO5NBRkf8CEJwgPX/D+iApALBme/jQmO+v9BcNnFNVk6E38enAjO9dU5GrP4KQVCYZXfDH8yAO4iRWYii7v07h+4oPDJswviZcg9obvKm0CnmJBMT3Zcsw7SgIIDsgBUidfXqpZtQmkXLADtzNYaFfNWpg03zaDkPMYMhQAiFWRLaRA1MinzxfSvv72PukgJJPHr9BZi/w==";
	CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	printf("发送导航数据3...  \n");

	//{"packet_id":9, "method" : "PutAudio", "business" : "", "audio_index" : 3, "audio_last" : 1, "audio" : ""}
	CloudVDPostData(session, "", 0, AUDIO_STATUS_LAST);
	printf("发送结束导航...  \n");

	// 导航业务开始
	audio = "JwDrAzI/QbvyJJYtTCVwLEnYRTeBcAr130CZAId2gMw25suRnQBSGkcsAO0tQJxtIFGagGfUbOzT+cPqg3NFTj7yRIxLgtFBcHJPqQIFOlX6Jm6py9bPKQDtLUCNxqtuDV4INnDP5UXPFGZFHyHW9fT48gRT4AYjkHOIHl6QzuWVPzsA7VooqFWvunctAf6zgD19tIj+nhMiJT1TljQuslWOb2VpFIIXwfosducYeXlevanrszNsdWxiMuDicPdaAOejAnIj9flhQsn9k+Sz/LCmEtGfZ7wS8Ck5srmiqxVOBRKQWkIsYFebNHNU8m+h52NY8RijMoWs27BAdCuUJGSwOJDguWETWxjFs6E4XjMAC+Rgd2gkmJTsm2sAwv4+uUX8g5VHWInyhcBtwQixk/DTLwwSGytRl+xx8XEBHcq3m1Kf2WOAPxii1NmSAWh6L8MYowKf8aJIE3aa4eDgseme2sdsft5pl05wyCcTbalWTmKS6rKD9+QBsin+xl0tKFzMcoT6lIdYAM4oau46bMAio4zPp9XYADVdqATrJ6WVvqXaakEnUpK7MYFaHrAr3IceWIYpEZ45Zu8ClSIUMjYcEqe9dIY4gVB2fl+g7A9SR95EZR9ob5XGqNXXhJhlB49JANO02DkI2eTkjabiXiUzmBKMDqIrXM5+X/2CYn+ftauO5wazHrdbk1N38o93Tfks01UtOlU0pRRqPwwHIDDnS8DykVuGmVaAPi8=";
	CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_FIRST);
	printf("发送导航雍和宫1...  \n");

	ResultData *rd;
	CloudVDGetResult(session, &rd);
	std::string res = UTF8ToGBK(rd->result);
	printf("%s \n", res.c_str());

	//audio = "PQDRwhVL4sFahnEBJJr6dRQCDGw3DvT+f4uCMJEtObfYXl57wzb4cSF2pGY2mRFk19Rus+ag94BJhm67YWk/TQDLp403McdEUC6/oO8umA4cKHHemuGnvQZH0O95d1XNgQcoQbMBEMonhmyOlVs/2oCMfEE1USob7cuWHpokvCFILob+NRjLlclB48L2/1gAyNF4EFrEteTEIOXnqtz2ZQx0PBBctMSevPPFR8TP89XI2CHxlDxvO8y3s7fLMPtIKyFHrjLwZN+FlPeshLao/v15G00W3uFCTLT8zreHOR7+c7TUq0xlY1MA41x1LeG2FOgh/dhTesHlwYfj26P/9l4K0qv8XEZuVR38hNee+M8eHk3j4vlgM/jPv/RmphC4F8rhf80M800OsRUDjMk/5aHqWurBg7epPObOZ39fAON5dPfa8aCWCRaP/5nWjQ92bNNRTmx6/VwRXFTNuvevkCu94nl5ZGKLxbrRQBp0p2LndVXaFzC3pr1duR/8LS9Viv47v0cv8p9BHUcL5lE2yam1398YEqYAmAZiKc3/WgDD9LJ6p0YUXgUBBzCFJ5oodC7dQkWHquH1pFa10wujza6yL23DS402Y6SBVAPiSuMS5DTiNV8q9U+F2BhdIDqjmekZXBfk2AHAuq9HYT4fDti3lJXh/CYCN08/AM41b72PSnR0fHEBECIs4CzkMWt4QHjdyOvmu3UWZ02y/w4YUykgdRlpUT9HzqgTulbJlae1tGfA9zQGaT+yXzYAz+OuExneM2CZcT342U7GIy1d8RDtwRgC1EBT2nZzATSId6zzKp0fL1LoXQ1FjjfqFNhYRX1/";
	//CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	//printf("发送导航雍和宫2...  \n");

	//audio = "WADI7FHqNZ/sX/QOkFPkZkGxnvRSa4NropG3533bODueXwfh5zbRBaVxCUvSwSHgGY6IrwN2baAcPiKMEYwAmaXbBbHh3grksAhY+cogDBOAY9z7m4q5iu0HUADEBWK7GZ+a8Ska1V1QUgimOgnbk+fzxdR3vPRC2dNQ4yTaYkB+e0zOYpbKe3bj06SoNITzA0/0qrTtIkQFd4XSLMtffmRJvCE2r3h51E6pf0QAvpP5PJQfN5BhG+2gZ9E6y09FiBq0RAw17Hzm/j5SjCyYpAf/PsrMKOX0VIU0h4H1NeqvD3TCc/P12Rgi81Oh/Zx3AP8yAPA7m8cfg2f6Vq7XGEtIgsiBdG2hI+u2ZAjTaGDpR4vQHztZu8C54FfMKVWgY08vC8IfZgC/LhRweSNwmNWhKcSbb9KnRazsANTmgFNiVCBm7NLTU+xrelRQr5AUDSF9qmobT3JgFwvNGff2uqkV+OrMrfx4LPMakLvh9iwOJFfgMglyY8w3rZqT9Un60nJERdLIVbEk7tE8Mx9bAMnBiQV2bElVFX/EdCPlrsM2RLdVRis3LxmOZKLij32zbsnGzNHyQoOMhSgMaimQQ20Mew+qyZC08Hz9wIFIPKvTzC+xq+C6U/osEV2x02O9dkwctYy/pzuXqn9PAM9ISZgD0+cbCxok9vHfy7tP7x27OD/Pgb1r7RK8T8GFxcLEZQ/3ExGj7rw8f5ETmXQoCXvDkJqv3AZeT3WfA82delZkYhajWIhlV4xopD83AM7rCMQIK452LEdjSICE8nzcq+XUlwn7agXWp3RSJTD97y0MRgEFxRCu8IQtYnjrKUwjJnWTWj8=";
	//CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	//printf("发送导航雍和宫3...  \n");

	//audio = "LwDNgFp0ChC3Df8Aeu3f+XZ4AEtKsVJ4BqDqVnXRodD37rzlwJpMRBVFIhoy7FYIX0YAy5p1K0cGsjSm9SdaA9w72yA/dIweEWfbd+YHRzGKWbp08KoFQdQ7vFWhgkqRj9IloK5UPGVZWZVEJHMF1r6RoqPZMGGJf0sAxk2JyRcNTsb6d1iYnDFUOzxBSrnVbcgzK7FJrF0PyPxUQ9zzN9oJIOK519eLZEuqlQbf1muns4axfviSiS7ln6mAL7Lw1oRuq8b/LQDwLYG8/mIMsCvYxtW52N6ZtCncXoWlSdXViUl+boqOVJlhZ60RsQKRZsb8hQMnAO2uicvg+Duty0i5rczu/3K5knoguiiX0Xsq//F2LC3olIZPpt2k/ykA7Tu1/Igl9s0H1IbMeNliAE8+yI5KyRA5Pg5ICF3HMLozoiIsV8c9J5smAO07tf+VP9qmtQp6MLpcnGf7Z0Fa1x5jdI/9i530pJlKBS6FgC//KQDtO7X4ldeGP0qucjHXhDwSnAg7EkE4gWs4E6+1tuO6TElxfy25V1GWnw==";
	//CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	//printf("发送导航雍和宫4...  \n");


	//audio = "JwDtBLBleIMd8RYcZS48cYMYq7clfJH//tWXvR5tOmPKbNtUqto034cmAOyiwGaW9DDSlJWyeRxI+8UNAt2MuCdEUVsvNFeNRGC+oa/lkaf/KwDsosBqkB9TnGDVcpCl9fy1MizAjNH4nlTk3r6ujcuqG9eXIMVT/STMqk5/LQDsosBnFvRiZWGOYKiWVrk0gwAqnwyHSHHa9eqGonkk2O0gVBiNoPDGBtPMRR8lAOyXc2IgPrBJ75PY3jPdjbyZRsmAORZOQmLGo1EOJ2pxfwTkLE8mAOyiwICMpV3Nx5+QjHytv6kEDbpWn4qfLVFUnqyA6hDZWz1BDf1/LADsosB5qACXg2H7+T21G51QrAaTvlHjfUP6PSqnYeHW3xMYw397pFp4EUuk/ysA7JNgnR5LttF9M0GcC2MOh43CamWW9PtyOXGitm6f4JyKveK+l5QcXYCprw==";
	//CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	//printf("发送导航雍和宫5...  \n");


	//audio = "KwDsk23I1XVrC4Qw0it9JdayeI7K56ur2wyWrnls//fg3VdsmyhFZ53++OT/JwDsosBm/dhftuN97XZIaefcU9IYofNjg9GHefbkyGD/xgvS23yTWqknAOyTYKgTAgQICiTzRI+YRRTzlwAWTO2r4e2MNeMQ+E4gCdk+jXQvvyMA7KLAZvdkmGMq1oRfKgY9clLbtEyGn8sLsKh5tMdPDQqinbcfAOyiwGXvsdEDuD3R3BGlaJ2HPdIN0fV5sYNT+bw2hV8+AO1v3MZn0N4b0jnTfVJr45OmDLw/RYlgcD7M4bcEY5hIN0EGw4zfZJT2flosRvdUQHPinhRECUy3sTFdzX5bKwDvLHJVOoPN8HEJ3Pc0Pmllq2R3xb2Rj9GC//1aSBxeY8KrPR7T7Ln6AHJ/KQDtvpl17IVjZc+bBXLtGLk16UkHAT+M6etjpWtTzsP6mUOsD0IaFxd8aQ==";
	//CloudVDPostData(session, audio, (int)strlen(audio), AUDIO_STATUS_CONTINUE);
	//printf("发送导航雍和宫6...  \n");

	//CloudVDPostData(session, "", 0, AUDIO_STATUS_LAST);
	//printf("发送结束导航到雍和宫...  \n");

	//CloudVDGetResult(session, &rd);
	//res = UTF8ToGBK(rd->result);
	//printf("%s \n",res.c_str());

	CloudVDEndSession(session);
	printf("结束Seesion...  \n");
	CloudVDFini();
	printf("程序退出..  \n");
	// 
	return 0;
}
