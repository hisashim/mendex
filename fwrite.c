#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exkana.h"
#include "exvar.h"

int line_length=0;

char *convcode();
int convputs();

int indwrite(filename,ind,pagenum)
/*   write ind file   */
char *filename;
struct index *ind;
int pagenum;
{
	int i,j,k,cc,hpoint=0;
	unsigned char datama[256],lbuff[4096],tmp[4096];
	FILE *fp;

	if (filename[0]!='\0') fp=fopen(filename,"w");
	else fp=stdout;

	convert(atama,datama);
	fputs(preamble,fp);

	if (fpage>0) {
		fprintf(fp,"%s%d%s",setpage_prefix,pagenum,setpage_suffix);
	}

	for (i=line_length=0;i<lines;i++) {
		if (i==0) {
			if (!((alphabet(ind[i].dic[0][0]))||(japanese(ind[i].dic[0])))) {
				if (lethead_flag) {
					if (symbol_flag && strlen(symbol)) {
						fprintf(fp,"%s%s%s",lethead_prefix,symbol,lethead_suffix);
					}
					else if (lethead_flag>0) {
						fprintf(fp,"%s%s%s",lethead_prefix,symhead_positive,lethead_suffix);
					}
					else if (lethead_flag<0) {
						fprintf(fp,"%s%s%s",lethead_prefix,symhead_negative,lethead_suffix);
					}
				}
				sprintf(lbuff,"%s%s",item_0,convcode(ind[i].idx[0]));
			}
			else if (alphabet(ind[i].dic[0][0])) {
				if (lethead_flag>0) {
					fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0],lethead_suffix);
				}
				else if (lethead_flag<0) {
					fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0]+32,lethead_suffix);
				}
				sprintf(lbuff,"%s%s",item_0,convcode(ind[i].idx[0]));
			}
			else if (japanese(ind[i].dic[0])) {
				if (lethead_flag) {
					fputs(lethead_prefix,fp);
					for (j=hpoint;j<(strlen(datama)/2);j++) {
						if (ind[i].dic[0][1]<datama[j*2+1]) {
							sprintf(tmp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
							convputs(tmp,fp);
							hpoint=j;
							break;
						}
					}
					if (j==(strlen(datama)/2)) {
						sprintf(tmp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
						convputs(tmp,fp);
					}
					fputs(lethead_suffix,fp);
				}
				sprintf(lbuff,"%s%s",item_0,convcode(ind[i].idx[0]));
				for (hpoint=0;hpoint<(strlen(datama)/2);hpoint++) {
					if (ind[i].dic[0][1]<datama[hpoint*2+1]) {
						break;
					}
				}
			}
			switch (ind[i].words) {
			case 1:
				sprintf(&lbuff[strlen(lbuff)],"%s",delim_0);
				break;

			case 2:
				sprintf(&lbuff[strlen(lbuff)],"%s%s",item_x1,convcode(ind[i].idx[1]));
				sprintf(&lbuff[strlen(lbuff)],"%s",delim_1);
				break;

			case 3:
				sprintf(&lbuff[strlen(lbuff)],"%s%s",item_x1,convcode(ind[i].idx[1]));
				sprintf(&lbuff[strlen(lbuff)],"%s%s",item_x2,convcode(ind[i].idx[2]));
				sprintf(&lbuff[strlen(lbuff)],"%s",delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
		else {
			if (!((alphabet(ind[i].dic[0][0]))||(japanese(ind[i].dic[0])))) {
				if ((alphabet(ind[i-1].dic[0][0]))||(japanese(ind[i-1].dic[0]))){
					fputs(group_skip,fp);
					if (lethead_flag && symbol_flag) {
						fprintf(fp,"%s%s%s",lethead_prefix,symbol,lethead_suffix);
					}
				}
			}
			else if (alphabet(ind[i].dic[0][0])) {
				if (ind[i].dic[0][0]!=ind[i-1].dic[0][0]) {
					fputs(group_skip,fp);
					if (lethead_flag>0) {
						fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0],lethead_suffix);
					}
					else if (lethead_flag<0) {
						fprintf(fp,"%s%c%s",lethead_prefix,ind[i].dic[0][0]+32,lethead_suffix);
					}
				}
			}
			else if (japanese(ind[i].dic[0])) {
				for (j=hpoint;j<(strlen(datama)/2);j++) {
					if ((ind[i].dic[0][0]<=datama[j*2])&&(ind[i].dic[0][1]<datama[j*2+1])) {
						break;
					}
				}
				if ((j!=hpoint)||(j==0)) {
					hpoint=j;
					fputs(group_skip,fp);
					if (lethead_flag!=0) {
						fputs(lethead_prefix,fp);
						sprintf(tmp,"%c%c",atama[(j-1)*2],atama[(j-1)*2+1]);
						convputs(tmp,fp);
						fputs(lethead_suffix,fp);
					}
				}

				else if ((strncmp(ind[i].dic[0],HIRAEND,2)>=0)&&(strncmp(ind[i-1].dic[0],ind[i].dic[0],2)!=0)) {
					fputs(group_skip,fp);
					if (lethead_flag!=0) {
						fputs(lethead_prefix,fp);
						sprintf(tmp,"%c%c",ind[i].dic[0][0],ind[i].dic[0][1]);
						convputs(tmp,fp);
						fputs(lethead_suffix,fp);
					}
				}
			}

			switch (ind[i].words) {
			case 1:
				sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_0,convcode(ind[i].idx[0]),delim_0);
				break;

			case 2:
				if (strcmp(ind[i-1].dic[0],ind[i].dic[0])!=0) {
					sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_0,convcode(ind[i].idx[0]),item_x1);
				}
				else {
					if (ind[i-1].words==1) {
						sprintf(&lbuff[strlen(lbuff)],"%s",item_01);
					}
					else {
						sprintf(&lbuff[strlen(lbuff)],"%s",item_1);
					}
				}
				sprintf(&lbuff[strlen(lbuff)],"%s",convcode(ind[i].idx[1]));
				sprintf(&lbuff[strlen(lbuff)],"%s",delim_1);
				break;

			case 3:
				if (strcmp(ind[i-1].dic[0],ind[i].dic[0])) {
					sprintf(&lbuff[strlen(lbuff)],"%s%s",item_0,convcode(ind[i].idx[0]));
					sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_x1,convcode(ind[i].idx[1]),item_x2);
				}
				else if (ind[i-1].words==1) {
					sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_01,convcode(ind[i].idx[1]),item_x2);
				}
				else if (strcmp(ind[i-1].dic[1],ind[i].dic[1])) {
					if (ind[i-1].words==2) sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_1,convcode(ind[i].idx[1]),item_12);
					else sprintf(&lbuff[strlen(lbuff)],"%s%s%s",item_1,convcode(ind[i].idx[1]),item_x2);
				}
				else {
					sprintf(&lbuff[strlen(lbuff)],"%s",item_2);
				}
				sprintf(&lbuff[strlen(lbuff)],"%s%s",convcode(ind[i].idx[2]),delim_2);
				break;

			default:
				break;
			}
			printpage(ind,fp,i,lbuff);
		}
	}
	fputs(postamble,fp);

	if (filename[0]!='\0') fclose(fp);
}

int printpage(ind,fp,num,lbuff)
/*   write page block   */
struct index *ind;
FILE *fp;
int num;
char *lbuff;
{
	int i,j,k,cc,eflg,cflg;
	char buff[4096],tmpbuff[4096],errbuff[4096],tmp[4096];

	buff[0]=tmpbuff[0]='\0';

	crcheck(lbuff,fp);
	line_length=strlen(lbuff);

	for(j=0;j<ind[num].num;j++) {
		cc=range_check(ind[num],j,lbuff);
		if (cc>j) {
			if (pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0])==pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])) {
				j=cc-1;
				continue;
			}
/* range process */
			if (ind[num].p[j].enc[0]==range_open
				|| ind[num].p[j].enc[0]==range_close)
				ind[num].p[j].enc++;
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(buff,"%s%s%s",encap_prefix,convcode(ind[num].p[j].enc),encap_infix);
			}
			if (strlen(suffix_3p)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))==2) {
				sprintf(&buff[strlen(buff)],"%s%s",convcode(ind[num].p[j].page),suffix_3p);
			}
			else if (strlen(suffix_mp)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))>=2) {
				sprintf(&buff[strlen(buff)],"%s%s",convcode(ind[num].p[j].page),suffix_mp);
			}
			else if (strlen(suffix_2p)>0 && (pnumconv(ind[num].p[cc].page,ind[num].p[cc].attr[0])-pnumconv(ind[num].p[j].page,ind[num].p[j].attr[0]))==1) {
				sprintf(&buff[strlen(buff)],"%s%s",convcode(ind[num].p[j].page),suffix_2p);
			}
			else {
				sprintf(&buff[strlen(buff)],"%s%s",convcode(ind[num].p[j].page),delim_r);
				sprintf(&buff[strlen(buff)],"%s",convcode(ind[num].p[cc].page));
			}
			sprintf(&tmpbuff[strlen(tmpbuff)],"%s",buff);
			buff[0]='\0';
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(&tmpbuff[strlen(tmpbuff)],"%s",encap_suffix);
			}
			linecheck(lbuff,tmpbuff);
			j=cc;
			if (j==ind[num].num) {
				goto PRINT;
			}
			else {
				sprintf(&tmpbuff[strlen(tmpbuff)],"%s",delim_n);
				linecheck(lbuff,tmpbuff);
			}
		}
		else if (strlen(ind[num].p[j].enc)>0) {
/* normal encap */
			if (ind[num].p[j].enc[0]==range_close) {
				sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
				for (i=0;i<ind[num].words;i++) sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind[num].idx[i]));
				fprintf(efp,"%s\n",errbuff);
				if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
				warn++;
				ind[num].p[j].enc++;
			}
			if (strlen(ind[num].p[j].enc)>0) {
				sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s%s",encap_prefix,convcode(ind[num].p[j].enc),encap_infix);
				sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s%s",convcode(ind[num].p[j].page),encap_suffix,delim_n);
				linecheck(lbuff,tmpbuff);
			}
			else {
				sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s",convcode(ind[num].p[j].page),delim_n);
				linecheck(lbuff,tmpbuff);
			}
		}
		else {
/* no encap */
			sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s",convcode(ind[num].p[j].page),delim_n);
			linecheck(lbuff,tmpbuff);
		}
	}

	if (ind[num].p[j].enc[0]==range_open) {
		sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
		for (k=0;k<ind[num].words;k++) sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind[num].idx[k]));
		fprintf(efp,"%s\n",errbuff);
		if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
		warn++;
		ind[num].p[j].enc++;
	}
	else if (ind[num].p[j].enc[0]==range_close) {
		sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
		for (k=0;k<ind[num].words;k++) sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind[num].idx[k]));
		fprintf(efp,"%s\n",errbuff);
		if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
		warn++;
		ind[num].p[j].enc++;
	}
	if (strlen(ind[num].p[j].enc)>0) {
		sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s%s",encap_prefix,convcode(ind[num].p[j].enc),encap_infix);
		sprintf(&tmpbuff[strlen(tmpbuff)],"%s%s",convcode(ind[num].p[j].page),encap_suffix);
	}
	else {
		sprintf(&tmpbuff[strlen(tmpbuff)],"%s",convcode(ind[num].p[j].page));
	}
	linecheck(lbuff,tmpbuff);

PRINT:
	fputs(lbuff,fp);
	fputs(delim_t,fp);
	lbuff[0]='\0';
}

int range_check(ind,count,lbuff)
struct index ind;
int count;
char *lbuff;
{
	int i,j,k,cc1,cc2,start,force=0;
	char buff[4096],tmpbuff[4096],errbuff[4096],tmp[4096];

	for (i=count;i<ind.num+1;i++) {
		if (ind.p[i].enc[0]==range_close) {
			sprintf(errbuff,"Warning: Unmatched range closing operator \'%c\',",range_close);
			sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind.idx[0]));
			fprintf(efp,"%s\n",errbuff);
			if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
			warn++;
			ind.p[i].enc++;
		}
		if (ind.p[i].enc[0]==range_open) {
			start=i;
			ind.p[i].enc++;
			for (j=i;j<ind.num+1;j++) {
				if (strcmp(ind.p[start].enc,ind.p[j].enc)) {
					if (ind.p[j].enc[0]==range_close) {
						ind.p[j].enc++;
						ind.p[j].enc[0]='\0';
						force=1;
						break;
					}
					else if (j!=i && ind.p[j].enc[0]==range_open) {
						sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
						for (k=0;k<ind.words;k++) sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind.idx[k]));
						fprintf(efp,"%s\n",errbuff);
						if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
						warn++;
						ind.p[j].enc++;
					}
					if (strlen(ind.p[j].enc)>0) {
						sprintf(tmpbuff,"%s%s%s",encap_prefix,convcode(ind.p[j].enc),encap_infix);
						sprintf(tmpbuff,"%s%s%s",convcode(ind.p[j].page),encap_suffix,delim_n);
						linecheck(lbuff,tmpbuff);
					}
				}
			}
			if (j==ind.num+1) {
					sprintf(errbuff,"Warning: Unmatched range opening operator \'%c\',",range_open);
					for (k=0;k<ind.words;k++) sprintf(&errbuff[strlen(errbuff)],"%s.",convcode(ind.idx[k]));
					fprintf(efp,"%s\n",errbuff);
					if (efp!=stderr) fprintf(stderr,"%s\n",errbuff);
					warn++;
			}
			i=j-1;
		}
		else if (prange && i<ind.num) {
			if (chkcontinue(ind.p,i)
				&& (!strcmp(ind.p[i].enc,ind.p[i+1].enc)
				|| ind.p[i+1].enc[0]==range_open))
				continue;
			else {
				i++;
				break;
			}
		}
		else {
			i++;
			break;
		}
	}
	cc1=pnumconv(ind.p[i-1].page,ind.p[i-1].attr[0]);
	cc2=pnumconv(ind.p[count].page,ind.p[count].attr[0]);
	if (cc1>=cc2+2 || (cc1>=cc2+1 && strlen(suffix_2p)) || force) {
		return i-1;
	}
	else return count;
}

int linecheck(lbuff,tmpbuff)
/*   check line length   */
char *lbuff,*tmpbuff;
{
	if (line_length+strlen(tmpbuff)>line_max) {
		sprintf(&lbuff[strlen(lbuff)],"\n%s%s",indent_space,tmpbuff);
		line_length=indent_length+strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
	else {
		sprintf(&lbuff[strlen(lbuff)],"%s",tmpbuff);
		line_length+=strlen(tmpbuff);
		tmpbuff[0]='\0';
	}
}

int crcheck(lbuff,fp)
char *lbuff;
FILE *fp;
{
	int i,j,cc;
	char buff[4096];

	for (i=strlen(lbuff);i>=0;i--) {
		if (lbuff[i]=='\n') {
			strncpy(buff,lbuff,i+1);
			buff[i+1]='\0';
			fputs(buff,fp);
			strcpy(buff,&lbuff[i+1]);
			strcpy(lbuff,buff);
			break;
		}
	}
}

char *convcode(buff)
/*   convert kanji code from EUC to output code   */
unsigned char *buff;
{
	int i,j,cc,jflag=0;
	static unsigned char buff2[4096];

	for (i=j=0;i<strlen(buff);i++,j++) {
		if (buff[i]=='\0') {
			break;
		}
		else if (buff[i]>=0x80) {
			if (kanji==Jis) {
				if (!jflag) {
					buff2[j++]=0x1b;
					buff2[j++]=0x24;
					buff2[j++]=0x42;
					jflag=1;
				}
				buff2[j++]=buff[i++]-0x80;
				buff2[j]=buff[i]-0x80;
			}
			else if (kanji==Sjis) {
				cc=buff[i]*256+buff[i+1]-0x8080;
				i++;
				cc=JIStoSJIS(cc);
				buff2[j++]=(cc>>8)&0xff;
				buff2[j]=cc&0xff;
			}
			else {
				buff2[j++]=buff[i++];
				buff2[j]=buff[i];
			}
		}
		else {
			if (jflag) {
				buff2[j++]=0x1b;
				buff2[j++]=0x28;
				buff2[j++]=0x42;
				jflag=0;
			}
			buff2[j]=buff[i];
		}
	}
	if (jflag) {
		buff2[j++]=0x1b;
		buff2[j++]=0x28;
		buff2[j++]=0x42;
		jflag=0;
	}
	buff2[j]='\0';
	return buff2;
}

int convputs(buff,fp)
/*   fputs width convert kanji code   */
char *buff;
FILE *fp;
{
	fputs(convcode(buff),fp);
}
