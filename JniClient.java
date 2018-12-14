package com.example.testndk;

public class JniClient {
	static public native String AddStr(String strA, String strB);
    static public native int AddInt(int a, int b);

	static public native int CloudVDInit(String configs);
    static public native int CloudVDStartSession(String params);
	// state 1: 表示开始 2:表示继续 3:表示结束
	static public native int CloudVDPostData(int session, String waveData,int len,int state);
	//  type 1: 表示结果 2：表示语音数据
    static public native String CloudVDGetResult(int session,int type);
	static public native int CloudVDEndSession(int session);
    static public native int CloudVDFini();
	
	/*
	String config = "{\"server_addr\":\"http://csngi2.vdialog.vcyber.com:10028/?v=2.0\",\"app_id\":\"cyw00036\",\"vin\":\"1234444\",\"mac\":\"123123123123\"}";
	CloudVDInit(config);
	printf("初始化...  \n");
	String param = "{\"business\":\"\",\"tts\":1,\"audio_format\":1,\"audio_quality\":10}";
	int  session = CloudVDStartSession(param);
	printf("Session开始...  \n");

	String audio = "LgDrn09g8sT0X5oAYFrFtBeo9d7NPHT7Y1cHSjT3XpKjdjwfruihtC4Sj9/iUYb/NgDtXaBJOvIpTj1GMXT0ZccaYXypWEAfqOVm3NFV+JW0acgpQmMafz7b8HkLiSGTFwN7bkH1n6tgANKCwz7txaQTGCo91FfS4XBM8j4z5Z0KAsb01S1UubTSFi4PZ6tuZEsRyiNWFjlAmyiDfml1StARhUfcVhaPm3YGRLgSLN3AMPQjd2JxTVZCGv4p4n06dJOauIdu5JA0714A2NgXkJuZPFekyC/6NMEUNbSB9zinPLrZow8TBeUMSOp/fxmsrMhxPBYZTkwRO0jkewP68vBIMGmgt16mJ8f1xDwEwqw73RhgEcctkf4VsYnh67trj9d143dFiyYkH1QAz00aq436fxyqF6gMTlBtwtqVbBVmzOVN0NTM2xxmkjwKDZQG0VVRMqLZZrdt4GKJaLx6nngaz1d0/7xCrDXrZJwyFNoUaNx9kl244FrWbezPd717TADQFrqjPlEm6yCftDnlytvz9BvKvvma1jZdjLHcnVu6RUjQF7kd8LxwiYkgFPT993HwzNhXDkJIahEymhVIAN6rua1XLsGo2q7detpHWQDPP7PBaJM/L8d11wH7yuBTB+E9O3fQFtn/1JIPC9SZLNTSTTkfwsiN2fOirX4RVpq4m1oUz6Ly6y3RVgt1pZt+EOdBOYahOM6H8MyyMU1P/goDmKkPl58JP2QAyjo3GDMm/bmvCSyueuydFD8Uz3UlIzI9J8FKLWEX6jcQG67qXfbb0VOZUmVjPn9lquHamvZALL9sKlWUA4BGCQ1VuzYqmx/JYbGDLq7ZBNFcT3eXra6fjqRVslqxYyrfDTGnvw==";
	CloudVDPostData(session, audio, audio.length(), 1);
	printf("发送导航数据1...  \n");

	audio = "ZwDj0Ea37/cVxXi0rQK6f/QhyyRLwqkzy8+3gKwRXNgigPgEtVyfNUjP8and+OWhtSzbs2gRzcaI2aklLpyqMaw56VZ6piWmYvG8eli8kU09gvRJzHJ4INhjigUn/4SL10kg+24t1nf/SgD1r4nKZ2cnvHsk9dJJSZpxUKWZlyBKnSxt+xzshOdSFLk0s3Z/wKyL8/7zJ8PkzoqjcHbcwI7sN3+q4emjUKV2LmK1gz+vUDwmf1wAx2UepGbC7hepzyBUOmHiJRcezfFhylb0o6YA+fhgF8Eg7jd98Ynnr5+YXo/wWKcoVUwSTVsRLbBOUC8P34z+socBPDd+b17GP9iM0+sZj6qITwZtpTcHdDfAvt88AMu3ZTdXdFc2+Cy7MftO/QCtiylKFmxosDxXx9hYLIr6QjQ06RjkElt6ZgUCm770+j79tifR0hfV35T1f1QAyNvY1YcTr9Vz2RT3uWE4RaR206m3I0mZDP76xZSkSJ1PbsxDpP8zOM5Nly9VNK96SN2dKLnZqEYlVBIPsKmFqlPi9eD1Xrc86hUIR1DnbrX00fKmVQDIPEmBWiqbGYCwTuOG0Z4qd8fGj5yVKWwot2IFccaoB4u9AdPr5K4NVxavQmSYRGaqCTLup+dzCpvebfxVN98EWZspuvVa5qQkiRTRA/CO69RuituPVQDIH8JiTp77xRyu1+LKEeHNyo93uamhzm40O9AfgWx6KOessu5tpyykHMpcIqUvqy3WHrD47RqCdT90x9XwebicHVbxN7f/J6CUEy9m5xdCGQV8PYa/VQDH4KmYhAuXgcrClKIWjaXFsnewbj57VVWGkJP2lJ9L1EPnstQxFFlo3mJSeu1pW5hy/AnVacpdAUQjl2FTb3lFpCfadmqm/dKr8OWKtITmEC+abEQX";
	CloudVDPostData(session, audio, audio.length(), 2);
	printf("发送导航数据2...  \n");

	audio = "TADEJpdHkWdd65DdOUZOK+iKG56mPUlR/KowqikyjVMwi1qoOsKnZyT8zhgPRj6kpzmoyHOFooSr2bvYio8RRYhttOnHITiINWZKm3THLgDyragG8UsKEQEyJq9qwPSaloy/SA3raPqXhKoY/17gpLtBCfgybues2nt9pAfPLgDvzWyhIpKh66JhcbEUA/9mGvhrtGAbLALFyruGsrAeJjGG2pxmhL3h+yl9T8u/MgDvMPmeUNQodjdaXaAvUD4IxDSxRKte6vHWn9n3wtyt1RZVMFwgu3A7VQoUISMei4TJQywA7sJZD/cs5X0cPT7OHUoNf0YjkG6BVBO4WjPyWbPsPzFaGTnCvntoIY9sO/80AO5NBRkf8CEJwgPX/D+iApALBme/jQmO+v9BcNnFNVk6E38enAjO9dU5GrP4KQVCYZXfDH8yAO4iRWYii7v07h+4oPDJswviZcg9obvKm0CnmJBMT3Zcsw7SgIIDsgBUidfXqpZtQmkXLADtzNYaFfNWpg03zaDkPMYMhQAiFWRLaRA1MinzxfSvv72PukgJJPHr9BZi/w==";
	CloudVDPostData(session, audio, audio.length(), 2);
	printf("发送导航数据3...  \n");

	CloudVDPostData(session, "", 0, 3);
	printf("发送结束导航...  \n");

	String res = CloudVDGetResult(session, 1);
	CloudVDEndSession(session);
	CloudVDFini();
	*/
}