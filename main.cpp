#include <stdio.h>
#include <stdlib.h>
#include <graphics.h>
#include <math.h>

#define maxNokta 1000


typedef struct
{
    float x,y; // Cember kordinatlar x ve y
    float r;   // Cember yari capi

} cember;



void kordinatSistemiCiz(cember);
void bSplineCiz(float [][2],int);   // B-Spline Egrisi cizici fonksiyon

float ikiNoktaMesafe(float,float,float,float); // iki nokta arasi mesafe hesaplama fonksiyonu
float bSplineFormul(float [],float,int,int); // B-Spline temel fonksiyonu


cember noktaKarsilastir(float[][2],int);    // min. cember hesabý icin ilk iki nokta hesaplama fonksiyonu
cember verilenNoktayaGoreKarsilastir(float[][2],int,int);   // min. cember hesabý icin disarida kalan noktaya gore hesap fonksiyonu
cember verilenIkiNoktayaGoreKarsilastir(float[][2],int,int,int); // min. cember hesabý icin disarida  iki kalan noktaya gore hesap fonksiyonu
cember minCemberOlustur(float,float,float,float,float,float); // // min. cember hesabý icin disarida kalan 3 noktaya gore cember olusturan fonksiyon

int main()
{
    FILE *dosya;
    float noktaKordinat[maxNokta][2]; // noktalarýn bulundugu matris

    if((dosya=fopen("data.txt","r"))==NULL)
    {
        printf("HATA : Dosya acilamadi !!");
    }
    else
    {
        int noktaSayi;
        fscanf(dosya, "%d", &noktaSayi); // nokta sayisi okuma

        for(int i=0; i<noktaSayi; i++)
        {
            fscanf(dosya, "%f %f", &noktaKordinat[i][0],&noktaKordinat[i][1]); //noktalari diziye atma
        }

        cember kapsayanCember; // cember olusturma
        kapsayanCember=noktaKarsilastir(noktaKordinat,noktaSayi); // cember hesabýna göre kapsayan cemberi bulup atama yapma

        initwindow( 840, 760, "Minimum Enclosing Circle & B-Spline",400,20);

        kordinatSistemiCiz(kapsayanCember); // Kordinat Sistemini Cizen fonksiyon

        int orjinX=getmaxx()/2;
        int orjinY=getmaxy()/2;

        float xDegeri=(orjinX+kapsayanCember.x*(float) 20);
        float yDegeri=(orjinY+kapsayanCember.y*(float) -20);
        float yaricap=kapsayanCember.r*(float) 20;
        setlinestyle(0,1,5);
        setcolor(3);
        circle(xDegeri+2,yDegeri+2,yaricap);


        setcolor(MAGENTA);
        for(int i=0; i<noktaSayi; i++)
        {
            arc(1+orjinX+noktaKordinat[i][0]*20,1+orjinY-noktaKordinat[i][1]*20,0,360,2); // Noktalarý gosterme islemi
            printf("{ %.2f,%.2f }\n",noktaKordinat[i][0],noktaKordinat[i][1]);
        }
        setcolor(4);
        arc(xDegeri,yDegeri,0,360,2);  //Merkezi gosteren nokta

        float x1,y1;
        x1=sin(45)*kapsayanCember.r*20;
        y1=cos(45)*kapsayanCember.r*20;
        setlinestyle(0,1,3);
        line(xDegeri,yDegeri,xDegeri+x1,yDegeri-y1); //yaricap cizgisi

        bSplineCiz(noktaKordinat,noktaSayi); //B-Spline cizim ve hesap fonksyionu

        getch();
    }
    return 0;
}
float ikiNoktaMesafe(float x0,float y0,float x1,float y1)
{
    float mesafe;
    mesafe=sqrt(pow(x0-x1, 2) + pow(y0 -y1, 2)); // iki nokta arasi hesap formulu

    return mesafe;
}
cember noktaKarsilastir(float noktalar[][2],int noktaSayisi)
{
    cember tempCember;
    tempCember.x = ((noktalar[0][0]+noktalar[1][0])/2); // ilk iki noktaya göre cember
    tempCember.y = ((noktalar[0][1]+noktalar[1][1])/2);
    tempCember.r = (ikiNoktaMesafe(noktalar[0][0],noktalar[0][1],noktalar[1][0],noktalar[1][1]))/2;

    int icindeMi;
    for(int i=2; i<noktaSayisi; i++) // ilk iki noktaya göre çember oluþtur
    {
        icindeMi = pow(noktalar[i][0] - tempCember.x, 2) + pow(noktalar[i][1] - tempCember.y, 2) - pow(tempCember.r, 2);

        if(icindeMi>0)//eger cikan sonuc 0dan buyukse cemberin disindadir
        {

            tempCember=verilenNoktayaGoreKarsilastir(noktalar,i,noktaSayisi);
        }
    }
    return tempCember;
}
cember verilenNoktayaGoreKarsilastir(float noktalar[][2],int disNoktaIndis,int noktaSayisi)
{
    cember tempCember;
    tempCember.x = ((noktalar[0][0]+noktalar[disNoktaIndis][0])/2); //dista kalan nokta ile ilk noktaya gore yeni cember
    tempCember.y = ((noktalar[0][1]+noktalar[disNoktaIndis][1])/2);
    tempCember.r = ikiNoktaMesafe(noktalar[0][0],noktalar[0][1],noktalar[disNoktaIndis][0],noktalar[disNoktaIndis][1])/2;

    float icindeMi;
    for(int i=1; i<disNoktaIndis; i++) // ilk iki noktaya göre çember oluþtur
    {
        icindeMi = pow(noktalar[i][0] - tempCember.x, 2) + pow(noktalar[i][1] - tempCember.y, 2) - pow(tempCember.r, 2);

        if(icindeMi>0)//eger cikan sonuc 0dan buyukse cemberin disindadir
        {
            //bu nokta da disinda ise iki noktaya göre hesap yapilir
            tempCember=verilenIkiNoktayaGoreKarsilastir(noktalar,i,disNoktaIndis,i);
        }
    }
    return tempCember;
}
cember verilenIkiNoktayaGoreKarsilastir(float noktalar[][2],int ilkNoktaIndis,int ikinciNoktaIndis,int noktaSayisi)
{
    cember tempCember;
    tempCember.x = ((noktalar[ilkNoktaIndis][0]+noktalar[ikinciNoktaIndis][0])/2); //dista kalan nokta ile ilk noktaya gore yeni cember
    tempCember.y = ((noktalar[ilkNoktaIndis][1]+noktalar[ikinciNoktaIndis][1])/2);
    tempCember.r = (ikiNoktaMesafe(noktalar[ilkNoktaIndis][0],noktalar[ilkNoktaIndis][1],noktalar[ikinciNoktaIndis][0],noktalar[ikinciNoktaIndis][1])/2);

    float icindeMi;
    for(int i=0; i<noktaSayisi; i++) // ilk iki noktaya göre çember oluþtur
    {
        icindeMi = pow(noktalar[i][0] - tempCember.x, 2) + pow(noktalar[i][1] - tempCember.y, 2) - pow(tempCember.r, 2);

        if(icindeMi>0)//eger cikan sonuc 0dan buyukse cemberin disindadir
        {
            //bu nokta da disinda ise iki noktaya göre hesap yapilir
            tempCember=minCemberOlustur(noktalar[ilkNoktaIndis][0],noktalar[ilkNoktaIndis][1],noktalar[ikinciNoktaIndis][0],noktalar[ikinciNoktaIndis][1],noktalar[i][0],noktalar[i][1]);
        }
    }
    return tempCember;
}
cember minCemberOlustur(float x0,float y0,float x1,float y1,float x2,float y2)    //verilen uc noktaya gore cember cizimi
{
    // Verilen uc noktaya göre cember hesabý
    // Bunu yapmak icin her noktaya gore cember denklemi yazilir
    // Ve elde edilen 3 bilinmeyenli denkelm cozulurek
    // Cember denkeminde ki D,E,F elde edilir
    // Bunlar ile de cemberin merkez noktalarý ve yarýcapýna ulaþýrýz.


    float a,b,c,d,e,f,g,k,l,M,n,r,s,x,y,z;

    /*
    ax+by+cz+d=0 1. denklem -> burada x = M a ise kat sayýsýsý anlamýna gelir, y=E b katsayýsý, cz+d=F
    ex+fy+gz+k=0 2. denklem -> burada x = M a ise kat sayýsýsý anlamýna gelir, y=E b katsayýsý  cz+d=F
    lx+ny+rz+s=0 3. denklem -> burada x = M a ise kat sayýsýsý anlamýna gelir, y=E b katsayýsý  cz+d=F
    */

    a=x0,b=y0,c=1.0; // 1. denklemin kat sayieari
    e=x1,f=y1,g=1.0; // 1. denklemin kat sayieari
    l=x2,n=y2,r=1.0; // 1. denklemin kat sayieari

    d=(x0*x0)+(y0*y0);  // 1. denklemde her nokta icin cember genel denkleminde ki x^2+y^2 kismi
    s=(x2*x2)+(y2*y2);  // 2. denklemde her nokta icin cember genel denkleminde ki x^2+y^2 kismi
    k=(x1*x1)+(y1*y1); //  3. denklemde her nokta icin cember genel denkleminde ki x^2+y^2 kismi

    M = ((a*f*r+b*l*g+c*e*n)-(a*g*n+b*e*r+c*f*l)); // Cramer metodunu kullanmak icin 3 denklemin olusturdugu matris'e determinant hesabi
    x = ((b*r*k+c*f*s+d*g*n)-(b*g*s+c*n*k+d*f*r))/M; // Cramer metodu kullanarak ilk bilinmeyenimiz D bulunur ve x eþitlenir
    y = ((a*g*s+c*l*k+d*e*r)-(a*r*k+c*e*s+d*g*l))/M; // Cramer metodu kullanarak ikinci bilinmeyenimiz E bulunur ve y eþitlenir
    z = ((a*n*k+b*e*s+d*f*l)-(a*f*s+b*l*k+d*e*n))/M; // Cramer metodu kullanarak ucuncu bilinmeyenimiz F bulunur ve z eþitlenir


    float xNokta,yNokta,yariCap;

    xNokta=x/2.0;
    yNokta=y/2.0;
    yariCap=(sqrt(pow(x,2)+pow(y,2)-4*z))/2.0;


    xNokta=xNokta*-1.0;
    yNokta=yNokta*-1.0;

    cember minCember;
    minCember.x=xNokta;
    minCember.y=yNokta;
    minCember.r=yariCap;

    return minCember;
}
float bSplineFormul(float knotArr[], float u, int i, int derece)
{
    //B-Spline eðrilerine ait temel fonksyionu kullanýlarak gerekli hesaplar yapýlýr.
    if(derece==1)
    {
        if(knotArr[i]<=u && u<=knotArr[i+1])
            return 1; //formülde belirtlilen gerekli durdurma,eþitlik kýsmý
        else
            return 0;
    }
    float sonuc;
    sonuc = ((u-knotArr[i])/(knotArr[i+derece-1]-knotArr[i])*bSplineFormul(knotArr, u, i, derece-1)) + // öz yinelemeli olan formüle gerekli deðerler girilir
                ((knotArr[i+derece]-u)/(knotArr[i+derece]-knotArr[i+1])*bSplineFormul(knotArr, u, i+1, derece-1));
    return sonuc;
}
void bSplineCiz(float noktalar[][2],int noktaSayisi)
{
    int noktaSayi,derece;
    derece=2;
    noktaSayi = noktaSayisi;

    float knotArr[noktaSayi+derece];
    int i;
    for(i=0; i<noktaSayi+derece; i++)
    {
        knotArr[i]=(((double)i)/(noktaSayi+derece-1)); // b-spline icin gecerli olan knot dizisi olusturulur
    }

    double x, y, basis, u;
    for(u=knotArr[derece-1]; u<=knotArr[noktaSayi]; u+=0.0001)
    {
        x = 0;
        y = 0;
        for(i=0;i<noktaSayi;i++)
        {
            basis = bSplineFormul(knotArr, u, i, derece);
            x += basis*noktalar[i][0]*20.0;
            y -= basis*noktalar[i][1]*20.0;
        }
        setlinestyle(0,0,3);
        putpixel(x+420.0,y+380.0, YELLOW); // B-Spline egrisi cizilir
    }
}
void kordinatSistemiCiz(cember kapsayanCember)
{
    int orjinX=getmaxx()/2;
    int orjinY=getmaxy()/2;

    setcolor(8);
    setlinestyle(1,1,0);
    for(int pos=0; pos<getmaxx(); pos+=20)
    {
        line(0,pos,getmaxx(),pos);
        line(pos,0,pos,getmaxy());
    }

    setlinestyle(0,1,3);
    setcolor(WHITE);
    line(orjinX,0,orjinX,getmaxy());
    line(0,orjinY,getmaxx(),orjinY);


    char xDuzlem[60];
    float artisPozitif=0.0;
    for(int i=0; i<21; i++)
    {
        sprintf(xDuzlem,"%d",i);
        if(i==0)
        {
            printf(" ");
        }
        else
        {
            outtextxy(orjinX+artisPozitif-5,orjinY+5,xDuzlem);
            outtextxy(orjinX+5,orjinY-artisPozitif-7,xDuzlem);
        }
        artisPozitif+=20.5;
    }

    char yDuzlem[60];
    float artisNegatif=0.0;

    for(int i=0; i>-21; i--)
    {
        sprintf(yDuzlem,"%d",i);
        if(i==0)
        {
            printf(" ");
        }
        else
        {
            outtextxy(orjinX-artisNegatif,orjinY+5,yDuzlem);
            outtextxy(orjinX+4,orjinY+artisNegatif-7,yDuzlem);
        }
        artisNegatif+=20.5;
    }
    setcolor(WHITE);
    char buf[128];
    sprintf(buf, "Merkez: = ( x = %.2f , y = %.2f )", kapsayanCember.x,kapsayanCember.y);
    outtextxy(orjinX-410,orjinY-340, buf);
    sprintf(buf, "Yaricap: = ( r = %.2f )", kapsayanCember.r);
    outtextxy(orjinX-410,orjinY-320, buf);

    printf("\nMerkez: = ( x = %.2f , y = %.2f )\n", kapsayanCember.x,kapsayanCember.y);
    printf("Yaricap: = ( r = %.2f )\n", kapsayanCember.r);
}
