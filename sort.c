#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exkana.h"
#include "exvar.h"

int sym,number,alpha,kana;

wsort(ind,num)
/*   sort index   */
struct index *ind;
int num;
{
	int i,j,order,wcomp();

	for (order=1,i=0;;i++) {
		switch (character_order[i]) {
		case '\0':
			goto BREAK;
			break;

		case 'S':
		case 's':
			sym=order++;
			break;

		case 'N':
		case 'n':
			number=order++;
			break;

		case 'E':
		case 'e':
			alpha=order++;
			break;

		case 'J':
		case 'j':
			kana=order++;
			break;

		default:
			break;
		}
	}

BREAK:

	if (sym==0) sym=order++;
	if (number==0) number=order++;
	if (alpha==0) alpha=order++;
	if (kana==0) kana=order++;

	qsort(ind,num,sizeof(struct index),wcomp);
}

int wcomp(index1,index2)
/*   compare for sorting index   */
struct index *index1,*index2;
{
	int i,j,k,cc,prechar,pri1,pri2;

	scount++;

	for (j=0;j<3;j++) {

/*   check level   */
		if (((*index1).words==j)&&((*index2).words!=j)) return -1;
		else if (((*index1).words!=j)&&((*index2).words==j)) return 1;

		for(i=0;;i++) {

/*   even   */
			if (((*index1).dic[j][i]=='\0')&&((*index2).dic[j][i]=='\0')) break;

/*   index1 is shorter   */
			if ((*index1).dic[j][i]=='\0') return -1;

/*   index2 is shorter   */
			if ((*index2).dic[j][i]=='\0') return 1;

/*   priority   */
			if ((priority!=0)&&(i>0)) {
				if (prechar==0) {
					if ((japanese(&(*index1).dic[j][i]))
						&&(!japanese(&(*index2).dic[j][i])))
						return -1;

					if ((japanese(&(*index2).dic[j][i]))
						&&(!japanese(&(*index1).dic[j][i])))
						return 1;
				}
				else {
					if ((japanese(&(*index1).dic[j][i]))
						&&(!japanese(&(*index2).dic[j][i])))
						return 1;

					if ((japanese(&(*index2).dic[j][i]))
						&&(!japanese(&(*index1).dic[j][i])))
						return -1;
				}
			}

/*   compare group   */
			if (ordering(&(*index1).dic[j][i])<ordering(&(*index2).dic[j][i]))
				return -1;

			if (ordering(&(*index1).dic[j][i])>ordering(&(*index2).dic[j][i]))
				return 1;

/*   symbol pattern   */
			if ((!numeric((*index1).dic[j][i]))&&(numeric((*index2).dic[j][i])))
				return -1;

			if ((!numeric((*index2).dic[j][i]))&&(numeric((*index1).dic[j][i])))
				return 1;

/*   simple compare   */
			if ((*index1).dic[j][i]<(*index2).dic[j][i]) return -1;
			else if ((*index1).dic[j][i]>(*index2).dic[j][i]) return 1;

/*   2byte character   */
			if (((*index1).dic[j][i]>=0x80)&&((*index2).dic[j][i]>=0x80)) {
				prechar=1;
				i++;
				if ((*index1).dic[j][i]<(*index2).dic[j][i]) return -1;
				else if ((*index1).dic[j][i]>(*index2).dic[j][i]) return 1;
			}
			else prechar=0;
		}

/*   compare index   */
		for (i=0;;i++) {
			if (((*index1).idx[j][i]=='\0')&&((*index2).idx[j][i]=='\0')) break;
			else if ((*index1).idx[j][i]=='\0') return -1;
			else if ((*index2).idx[j][i]=='\0') return 1;
			else if ((*index1).idx[j][i]<(*index2).idx[j][i]) return -1;
			else if ((*index1).idx[j][i]>(*index2).idx[j][i]) return 1;
		}
	}
	return 0;
}

int pagesort(ind,num)
/*   sort page   */
struct index *ind;
int num;
{
	int i,j,k,cc;
	struct page *buff;

	int pcomp();

	for (i=0;i<num;i++) {
		if (ind[i].num==0) continue;

		buff=(struct page *)malloc(sizeof(struct page)*(ind[i].num+1));
		for (j=0;j<=ind[i].num;j++) {
			buff[j]=ind[i].p[j];
		}
		qsort(buff,ind[i].num+1,sizeof(struct page),pcomp);
		for (j=0;j<=ind[i].num;j++) {
			ind[i].p[j]=buff[j];
		}
		free(buff);
	}
}

int pcomp(page1,page2)
/*   compare for sorting page   */
struct page *page1,*page2;
{
	int i,j,cc=0,num1,num2;
	char buff[16];

	scount++;

	for (i=0;i<3;i++) {
		if ((page1->attr[i]<0)&&(page2->attr[i]<0)) return 0;
		else if ((page1->attr[i]<0)&&(page2->attr[i]>=0)) return -1;
		else if ((page2->attr[i]<0)&&(page1->attr[i]>=0)) return 1;

		if (page1->attr[i]>page2->attr[i]) return 1;
		if (page1->attr[i]<page2->attr[i]) return -1;

		for (j=cc;j<strlen(page1->page);j++) {
			if (strncmp(&page1->page[j],page_compositor,strlen(page_compositor))==0) break;
		}
		strncpy(buff,&page1->page[cc],j-cc);
		buff[j-cc]='\0';
		num1=pnumconv(buff,page1->attr[i]);

		for (j=cc;j<strlen(page2->page);j++) {
			if (strncmp(&page2->page[j],page_compositor,strlen(page_compositor))==0) break;
		}
		strncpy(buff,&page2->page[cc],j-cc);
		buff[j-cc]='\0';
		num2=pnumconv(buff,page2->attr[i]);

		if (num1>num2) return 1;
		else if (num1<num2) return -1;

		if (page1->enc[0]=='(' || page2->enc[0]==')') return -1;
		if (page1->enc[0]==')' || page2->enc[0]=='(') return 1;

		cc=j+strlen(page_compositor);
	}

	return 0;
}

int ordering(buff)
unsigned char *buff;
{
	if ((*buff)<0x80) {
		if (alphabet(*buff)) return alpha; 
		else if (numeric(*buff)) return number; 
		else return sym;
	}
	else {
		if (japanese(buff)) return kana;
		else return sym;
	}
}

int alphanumeric(c)
unsigned char c;
{
	if (((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z'))||((c>='0')&&(c<='9')))
		return 1;
	else return 0;
}

int alphabet(c)
unsigned char c;
{
	if (((c>='A')&&(c<='Z'))||((c>='a')&&(c<='z'))) return 1;
	else return 0;
}

int numeric(c)
unsigned char c;
{
	if ((c>='0')&&(c<='9')) return 1;
	else return 0;
}

int japanese(buff)
unsigned char *buff;
{
	if (strncmp(buff,HIRATOP,2)>=0) return 1; 
	else return 0;
}

int chkcontinue(p,num)
struct page *p;
int num;
{
	int i,j,cc=0,num1,num2;
	char buff[16];

	for (i=0;i<3;i++) {
		if ((p[num].attr[i]<0)&&(p[num+1].attr[i]<0)) return 1;
		else if (p[num].attr[i]!=p[num+1].attr[i]) return 0;

		for (j=cc;j<strlen(p[num].page);j++) {
			if (strncmp(&p[num].page[j],page_compositor,strlen(page_compositor))==0) break;
		}
		strncpy(buff,&p[num].page[cc],j);
		buff[j]='\0';
		num1=pnumconv(buff,p[num].attr[i]);

		for (j=cc;j<strlen(p[num+1].page);j++) {
			if (strncmp(&p[num+1].page[j],page_compositor,strlen(page_compositor))==0) break;
		}
		strncpy(buff,&p[num+1].page[cc],j);
		buff[j]='\0';
		num2=pnumconv(buff,p[num+1].attr[i]);

		if (num1==num2 || num1+1==num2) {
			if (i==2) return 1;
			if ((p[num].attr[i+1]<0)&&(p[num+1].attr[i+1]<0)) return 1;
			else return 0;
		}
		else if (num1!=num2) return 0;

		cc=j+strlen(page_compositor);
	}

	return 1;
}
