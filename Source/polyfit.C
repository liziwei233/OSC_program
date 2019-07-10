#include<bits/stdc++.h>
using namespace std;
const int maxn = 100;
double x[maxn][maxn],y[maxn][maxn],maze[maxn][maxn],w[maxn],inv[maxn][maxn];
double yy[maxn],a[maxn];
//按第一行展开计算|A|
double getA(double arcs[maxn][maxn],int n)
{
    if(n==1)
    {
        return arcs[0][0];
    }
    double ans = 0;
    double temp[maxn][maxn]={0.0};
    int i,j,k;
    for(i=0;i<n;i++)
    {
        for(j=0;j<n-1;j++)
        {
            for(k=0;k<n-1;k++)
            {
                temp[j][k] = arcs[j+1][(k>=i)?k+1:k];

            }
        }
        double t = getA(temp,n-1);
        if(i%2==0)
        {
            ans += arcs[0][i]*t;
        }
        else
        {
            ans -=  arcs[0][i]*t;
        }
    }
    return ans;
}
//计算每一行每一列的每个元素所对应的余子式，组成A*
void  getAStart(double arcs[maxn][maxn],int n,double ans[maxn][maxn])
{
    if(n==1)
    {
        ans[0][0] = 1;
        return;
    }
    int i,j,k,t;
    double temp[maxn][maxn];
    for(i=0;i<n;i++)
    {
        for(j=0;j<n;j++)
        {
            for(k=0;k<n-1;k++)
            {
                for(t=0;t<n-1;t++)
                {
                    temp[k][t] = arcs[k>=i?k+1:k][t>=j?t+1:t];
                }
            }


            ans[j][i]  =  getA(temp,n-1);
            if((i+j)%2 == 1)
            {
                ans[j][i] = - ans[j][i];
            }
        }
    }
}
//得到给定矩阵src的逆矩阵保存到des中。
bool GetMatrixInverse(double src[maxn][maxn],int n,double des[maxn][maxn])
{
    double flag=getA(src,n);
    double t[maxn][maxn];
    if(flag==0)
    {
        return false;
    }
    else
    {
        getAStart(src,n,t);
        for(int i=0;i<n;i++)
        {
            for(int j=0;j<n;j++)
            {
                des[i][j]=t[i][j]/flag;
            }

        }
    }
    return true;
}
int polyfit(int m= 10, int n=1)
{
    //int m,n;
    // freopen("txt/li4-1.txt","r",stdin);
    // freopen("txt/li4-1_out.txt","w+",stdout);
    //freopen("txt/ti4-1.txt","r",stdin);
    //freopen("txt/ti4-1_out.txt","w+",stdout);
    freopen("Source/fitdata.txt","r",stdin);
    freopen("fitdata_out.txt","w+",stdout);
    while(~scanf("%d",&m)) {
        for(int i=0;i<m;i++) x[i][0] = 1;
        for(int i=0;i<m;i++) y[i][0] = 1;
        for(int i=0;i<m;i++) scanf("%lf",&x[i][1]);
        for(int i=0;i<m;i++) scanf("%lf",&y[i][1]);
        for(int i=0;i<m;i++) scanf("%lf",&w[i]);
        //printf("Weidu :\n");
        scanf("%d",&n);
        for(int i=2;i<2*n-1;i++) {
            for(int j=0;j<m;j++) {
                x[j][i] = x[j][i-1]*x[j][1];
                y[j][i] = y[j][i-1]*y[j][1];
            }
        }
        printf("矩阵 X[][]\n");
        for(int i=0;i<n;i++) {
            for(int j=0;j<n;j++) {
                maze[i][j] = 0;
                for(int k=0;k<m;k++) {
                    maze[i][j] += w[k]*x[k][i+j];
                }
                printf("%.2f ",maze[i][j]);
            }
            printf("\n");
        }
        printf("矩阵 Y[][]\n");
        for(int i=0;i<n;i++) {
            yy[i] = 0;
            for(int j=0;j<m;j++) {
                yy[i] += w[j]*x[j][i]*y[j][1];
            }
            printf("%.2f\n",yy[i]);
        }
        printf("矩阵 X[][]的逆\n");
        if(GetMatrixInverse(maze,n,inv)) {
            for(int i=0;i<n;i++) {
                for(int j=0;j<n;j++) printf("%.2f ",inv[i][j]);
                printf("\n");
            }
        }
        printf("系数矩阵 a[][]\n");
        for(int i=0;i<n;i++) {
            a[i] = 0;
            for(int j=0;j<n;j++) {
                a[i] += inv[i][j] * yy[j];
            }
            printf("%.2f\n",a[i]);
        }
    }
    return 0;
}