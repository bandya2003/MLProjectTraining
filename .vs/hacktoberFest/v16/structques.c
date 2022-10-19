#include<stdio.h>
#include<string.h>
struct student
{
    char name[100];
    int roll;
    float cgpa;
};
int main()
{
    struct student s1;
    struct student s2;
    struct student s3;
    int r;
    float cgpa;
    char n[100];
    //for student 1
    printf("enter roll");
    scanf("%d",&r);
    printf("enter cgpa");
    scanf("%f",&cgpa);
    printf("enter name");
    scanf("%s",&n);
    s1.roll=r;
    s1.cgpa=cgpa;
    strcpy(s1.name,n);
    //for student 2
    printf("enter roll");
    scanf("%d",&r);
    printf("enter cgpa");
    scanf("%f",&cgpa);
    printf("enter name");
    scanf("%s",&n);
    s2.roll=r;
    s2.cgpa=cgpa;
    strcpy(s2.name,n);
    //for student 3
    printf("enter roll");
    scanf("%d",&r);
    printf("enter cgpa");
    scanf("%f",&cgpa);
    printf("enter name");
    scanf("%s",&n);
    s3.roll=r;
    s3.cgpa=cgpa;
    strcpy(s3.name,n);
    printf("name of first student %s\n",s1.name);
    printf("roll of first student %d\n",s1.roll);
    printf("cgpa of first student %f\n",s1.cgpa);
    printf("--------------------------------------------------------------------\n");
    printf("name of 2nd student %s\n",s2.name);
    printf("roll of 2nd student %d\n",s2.roll);
    printf("cgpa of 2nd student %f\n",s2.cgpa);
    printf("--------------------------------------------------------------------\n");
    printf("name of 3rd student %s\n",s3.name);
    printf("roll of 3rd student %d\n",s3.roll);
    printf("cgpa of 3rd student %f\n",s3.cgpa);

}