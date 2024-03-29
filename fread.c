#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exkana.h"
#include "exvar.h"

#define BUFSIZE 65535

char *mfgets();
extern char *convcode();

int idxread(filename,start)
/*   read idx file   */
char *filename;
int start;
{
	int i,j,k,l,m,n,cc,indent,wflg,flg,bflg=0,nest,esc,quo,eflg=0,pacc,preject;
	unsigned char buff[BUFSIZE],wbuff[BUFSIZE],estr[256],table[BUFSIZE],tbuff[256],*tmp1,*tmp2;
	FILE *fp;

	pacc=acc;
	preject=reject;

	if (filename==NULL) {
		fp=stdin;
		if (verb!=0) fputs("Scanning input file stdin.",stderr);
		if (efp!=stderr) fputs("Scanning input file stdin.",efp);
	}
	else {
		fp=fopen(filename,"r");
		if (fp==NULL) {
			sprintf(buff,"%s.idx",filename);
			fp=fopen(buff,"r");
			if (fp==NULL) {
				if (verb!=0)
					fprintf(stderr,"Warning: Couldn't find input file %s.\n",filename);
				if (efp!=stderr)
					fprintf(efp,"Warning: Couldn't find input file %s.\n",filename);
				warn++;
				return 1;
			}
			else strcpy(filename,buff);
		}
		if (verb!=0) fprintf(stderr,"Scanning input file %s.",filename);
		if (efp!=stderr) fprintf(efp,"Scanning input file %s.",filename);
	}

	for (i=start,n=1;;i++,n++) {
		if (!(i%100))
			ind=(struct index *)realloc(ind,sizeof(struct index)*(i+100));
LOOP:
		ind[i].lnum=n;
		if (mfgets(buff,sizeof(buff)-1,fp)==NULL) break;
		for (j=bflg=cc=0;j<strlen(buff);j++) {
			if (buff[j]!=' ' && buff[j]!='\n') cc=1;
			if (bflg==0) {
				if (strncmp(&buff[j],keyword,strlen(keyword))==0) {
					j+=strlen(keyword);
					bflg=1;
				}
			}
			if (bflg==1) {
				if (buff[j]==arg_open) {
					j++;
					break;
				}
			}
		}
		if (j==strlen(buff)) {
			if (cc) reject++;
			i--;
			continue;
		}
		indent=wflg=k=nest=esc=quo=0;

/*   analize words   */

		for (;;j++,k++) {
			if (buff[j]=='\n' || buff[j]=='\0') {
				if (verb!=0)
					fprintf(stderr,"\nWarning: Incomplete first argument in %s, line %d.",filename,ind[i].lnum);
				if (efp!=stderr)
					 fprintf(efp,"\nWarning: Incomplete first argument in %s, line %d.",filename,ind[i].lnum);
				warn++;
				n++;
				goto LOOP;
			}

			if (buff[j]==quote && esc==0 && quo==0) {
				k--;
				quo=1;
				continue;
			}

			if (quo==0 && buff[j]==escape) {
				esc=1;
			}

			if (quo==0) {
				if (esc==0 && buff[j]==arg_open) {
					nest++;
					wbuff[k]=buff[j];
					continue;
				}
				else if (esc==0 && buff[j]==arg_close && nest>0) {
					nest--;
					wbuff[k]=buff[j];
					continue;
				}
				if (nest==0) {
					if (buff[j]==level) {
						esc=0;
						if (indent>=2) {
							fprintf(efp,"\nError: Extra `%c\' in %s, line %d.",level,filename,ind[i].lnum);
							if (efp!=stderr) fprintf(stderr,"\nError: Extra `%c\' in %s, line %d.",level,filename,ind[i].lnum);
							eflg++;
							reject++;
							n++;
							goto LOOP;
						}
						ind[i].idx[indent]=(unsigned char *)malloc(k+1);
						if (ind[i].idx[indent]==NULL) {
							fprintf(stderr,"Malloc error.(ind[%d].idx[%d])\n",i,indent);
							exit(-1);
						}
						strncpy(ind[i].idx[indent],wbuff,k);
						ind[i].idx[indent][k]='\0';
						if (!wflg) ind[i].org[indent]=NULL;
						indent++;
						wflg=0;
						k= -1;
						continue;
					}
					else if (buff[j]==actual) {
						esc=0;
						if (wflg) {
							fprintf(efp,"\nError: Extra `%c\' in %s, line %d.",actual,filename,ind[i].lnum);
							if (efp!=stderr) fprintf(stderr,"\nError: Extra `%c\' in %s, line %d.",actual,filename,ind[i].lnum);
							eflg++;
							reject++;
							n++;
							goto LOOP;
						}
						ind[i].org[indent]=(unsigned char *)malloc(k+1);
						if (ind[i].org[indent]==NULL) {
							fprintf(stderr,"Malloc error.(ind[%d].org[%d])\n",i,indent);
							exit(-1);
						}
						strncpy(ind[i].org[indent],wbuff,k);
						ind[i].org[indent][k]='\0';
						wflg=1;
						k= -1;
						continue;
					}
					else if ((esc==0 && buff[j]==arg_close) || buff[j]==encap) {
						esc=0;
						if (buff[j]==encap) {
							j++;
							cc=getestr(&buff[j],estr);
							if (cc<0) {
								fprintf(efp,"\nBad encap string in %s, line %d.",filename,ind[i].lnum);
								if (efp!=stderr) fprintf(stderr,"\nBad encap string in %s, line %d.",filename,ind[i].lnum);
								eflg++;
								reject++;
								n++;
								goto LOOP;
							}
							j+=cc;
						}
						else estr[0]='\0';

						ind[i].idx[indent]=(unsigned char *)malloc(k+1);
						if (ind[i].idx[indent]==NULL) {
							fprintf(stderr,"Malloc error.(ind[%d].idx[%d])\n",i,indent);
							exit(-1);
						}
						strncpy(ind[i].idx[indent],wbuff,k);
						ind[i].idx[indent][k]='\0';
						if (strlen(ind[i].idx[indent])==0) {
							if (wflg) {
								strcpy(ind[i].idx[indent],ind[i].org[indent]);
							}
							else if (indent>0) {
								indent--;
							}
							else {
								if (verb!=0)
									fprintf(stderr,"\nWarning: Illegal null field in %s, line %d.",filename,ind[i].lnum);
								if (efp!=stderr)
									fprintf(efp,"\nWarning: Illegal null field in %s, line %d.",filename,ind[i].lnum);
								warn++;
								n++;
								goto LOOP;
							}
						}
						if (!wflg) {
							ind[i].org[indent]=NULL;
						}
						break;
					}
				}
				if (bcomp==1) {
					if (buff[j]==' ' || buff[j]=='\t') {
						esc=0;
						if (k==0) {
							k--;
							continue;
						}
						else if (buff[j+1]==' ' || buff[j+1]=='\t' || buff[j+1]==encap || buff[j+1]==arg_close || buff[j+1]==actual || buff[j+1]==level) {
							k--;
							continue;
						}
						else if (buff[j]=='\t') {
							wbuff[k]=' ';
							continue;
						}
					}
				}
			}
			else quo=0;

			wbuff[k]=buff[j];
			if (buff[j]!=escape) esc=0;
			if (buff[j]>=0x80) {
				wbuff[k+1]=buff[j+1];
				j++;
				k++;
			}
		}
		ind[i].words=indent+1;

/*   kana-convert   */

		for (k=0;k<ind[i].words;k++) {
			if (ind[i].org[k]==NULL) {
				cc=convert(ind[i].idx[k],table);
				if (cc==-1) {
					fprintf(efp,"in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr) fprintf(stderr,"in %s, line %d.",filename,ind[i].lnum);
					eflg++;
					reject++;
					n++;
					goto LOOP;
				}
				ind[i].dic[k]=(unsigned char *)malloc(strlen(table)+1);
				if (ind[i].dic[k]==NULL) {
					fprintf(stderr,"Malloc error.(ind[%d].dic[%d])\n",i,k);
					exit(-1);
				}
				strcpy(ind[i].dic[k],table);
			}
			else {
				cc=convert(ind[i].org[k],table);
				if (cc==-1) {
					fprintf(efp,"in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr) fprintf(stderr,"in %s, line %d.",filename,ind[i].lnum);
					eflg++;
					reject++;
					n++;
					goto LOOP;
				}
				ind[i].dic[k]=(unsigned char *)malloc(strlen(table)+1);
				if (ind[i].dic[k]==NULL) {
					fprintf(stderr,"Malloc error.(ind[%d].dic[%d])\n",i,k);
					exit(-1);
				}
				strcpy(ind[i].dic[k],table);
			}
		}
		acc++;

/*   page edit   */

		if (i==0) {
			ind[0].num=0;
			ind[0].p=(struct page *)malloc(sizeof(struct page)*16);
			for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
			if (buff[j]=='\n' || buff[j]=='\0') {
				if (verb!=0)
					fprintf(stderr,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
				if (efp!=stderr)
					fprintf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
				acc--;
				reject++;
				warn++;
				n++;
				goto LOOP;
			}
			j++;
			for (k=nest=0;;j++,k++) {
				if (buff[j]=='\n' || buff[j]=='\0') {
					if (verb!=0)
						fprintf(stderr,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr)
						fprintf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					goto LOOP;
				}
				if (buff[j]==arg_open)
					nest++;
				else if (buff[j]==arg_close) {
					if (nest==0) {
						table[k]='\0';	
						ind[0].p[0].page=malloc(strlen(table)+1);
						if (ind[0].p[0].page==NULL) {
							fprintf(stderr,"Malloc error.(ind[0].p[0].page)\n");
							exit(-1);
						}
						strcpy(ind[0].p[0].page,table);
						break;
					}
					else nest--;
				}
				else if (buff[j]>=0x80) {
					table[k]=buff[j];
					j++;
					k++;
				}
				table[k]=buff[j];
			}
			ind[0].p[0].enc=malloc(strlen(estr)+1);
			if (ind[0].p[0].enc==NULL) {
				fprintf(stderr,"Malloc error.(ind[0].p[0].enc)\n");
				exit(-1);
			}
			strcpy(ind[0].p[0].enc,estr);
			chkpageattr(&ind[0].p[0]);
		}
		else {
			for (l=0;l<i;l++) {
				flg=0;
				if (ind[i].words!=ind[l].words) continue;
				for (flg=1,m=0;m<ind[i].words;m++) {
					if (strcmp(ind[i].idx[m],ind[l].idx[m])!=0) {
						flg=0;
						break;
					}
					if (strcmp(ind[i].dic[m],ind[l].dic[m])!=0) {
						if (ind[i].org[m]!=NULL) tmp1=ind[i].org[m];
						else tmp1=ind[i].idx[m];

						if (ind[l].org[m]!=NULL) tmp2=ind[l].org[m];
						else tmp2=ind[i].idx[m];

						if (verb!=0)
							fprintf(stderr,"\nWarning: Sort key \"%s\" is different from previous key \"%s\" for same index \"%s\" in %s, line %d.",convcode(tmp1),convcode(tmp2),convcode(ind[i].idx[m]),filename,ind[i].lnum);
						if (efp!=stderr)
							fprintf(efp,"\nWarning: Sort key \"%s\" is different from previous key \"%s\" for same index \"%s\" in %s, line %d.",convcode(tmp1),convcode(tmp2),convcode(ind[i].idx[m]),filename,ind[i].lnum);
						warn++;
						flg=0;
						break;
					}
				}
				if (flg==1) break;
			}

			if (flg==1) {
				for (m=0;m<ind[i].words;m++) {
					free(ind[i].idx[m]);
					free(ind[i].dic[m]);
				}

				i--;
				for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
				if (buff[j]=='\n' || buff[j]=='\0') {
					if (verb!=0)
						fprintf(stderr,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr)
						fprintf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					i++;
					goto LOOP;
				}
				j++;
				for (k=nest=0;;j++,k++) {
					if (buff[j]=='\n' || buff[j]=='\0') {
						if (verb!=0)
							fprintf(stderr,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						if (efp!=stderr)
							fprintf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						warn++;
						n++;
						i++;
						goto LOOP;
					}
					if (buff[j]==arg_open)
						nest++;
					else if (buff[j]==arg_close) {
						if (nest==0) break;
						else nest--;
					}
					else if (buff[j]>=0x80) {
						table[k]=buff[j];
						j++;
						k++;
					}
					table[k]=buff[j];
				}

				table[k]='\0';	

				for (k=0;k<=ind[l].num;k++) {
					if (strcmp(ind[l].p[k].page,table)==0) {
						if (strcmp(ind[l].p[k].enc,estr)==0) break;
					}
				}

				if (k>ind[l].num) {
					ind[l].num++;
					if (!((ind[l].num)%16)) ind[l].p=(struct page *)realloc(ind[l].p,sizeof(struct page)*((int)((ind[l].num)/16)+1)*16);

					ind[l].p[ind[l].num].page=malloc(strlen(table)+1);	
					strcpy(ind[l].p[ind[l].num].page,table);

					ind[l].p[ind[l].num].enc=malloc(strlen(estr)+1);	
					strcpy(ind[l].p[ind[l].num].enc,estr);
					chkpageattr(&ind[l].p[ind[l].num]);
				}
			}
			else {
				ind[i].num=0;
				ind[i].p=(struct page *)malloc(sizeof(struct page)*16);
				for (;buff[j]!=arg_open && buff[j]!='\n' && buff[j]!='\0';j++);
				if (buff[j]=='\n' || buff[j]=='\0') {
					if (verb!=0)
						fprintf(stderr,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					if (efp!=stderr)
						fprintf(efp,"\nWarning: Missing second argument in %s, line %d.",filename,ind[i].lnum);
					acc--;
					reject++;
					warn++;
					n++;
					goto LOOP;
				}
				j++;
				for (k=nest=0;;j++,k++) {
					if (buff[j]=='\n' || buff[j]=='\0') {
						if (verb!=0)
							fprintf(stderr,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						if (efp!=stderr)
							fprintf(efp,"\nWarning: Incomplete second argument in %s, line %d.",filename,ind[i].lnum);
						acc--;
						reject++;
						warn++;
						n++;
						goto LOOP;
					}
					if (buff[j]==arg_open)
						nest++;
					if (buff[j]==arg_close) {
						if (nest==0) {
							table[k]='\0';	
							ind[i].p[0].page=malloc(strlen(table)+1);	
							strcpy(ind[i].p[0].page,table);
							break;
						}
						else nest--;
					}
					else if (buff[j]>=0x80) {
						table[k]=buff[j];
						j++;
						k++;
					}
					table[k]=buff[j];
				}
				ind[l].p[0].enc=malloc(strlen(estr)+1);	
				strcpy(ind[i].p[0].enc,estr);
				chkpageattr(&ind[i].p[0]);
			}
		}
	}
	lines=i;

	if (verb!=0) {
		fprintf(stderr,"...done (%d entries accepted, %d rejected).\n",acc-pacc, reject-preject);
	}
	if (efp!=stderr) {
		fprintf(efp,"...done (%d entries accepted, %d rejected).\n",acc-pacc, reject-preject);
	}
	return eflg;
}

int getestr(buff,estr)
/*   pic up encap string   */
unsigned char *buff,*estr;
{
	int i,j,cc,nest=0;

	for (i=0;i<strlen(buff);i++) {
		if (buff[i]==encap) {
			if (i>0) {
				if (buff[i-1]<0x80) {
					estr[i]=buff[i];
					i++;
				}
			}
			else {
				estr[i]=buff[i];
				i++;
			}
		}
		if (nest==0 && buff[i]==arg_close) {
			estr[i]='\0';
			return i;
		}
		if (buff[i]==arg_open) nest++;
		else if (buff[i]==arg_close) nest--;
		estr[i]=buff[i];
		if (buff[i]>0x80) {
			i++;
			estr[i]=buff[i];
		}
	}

	return -1;
}

int sstrcmp(buff1,buff2)
char *buff1,*buff2;
{
	int i;

	for (i=0;;i++) {
		if (buff1[i]=='\0' && buff2[i]=='\0') return 0;
		if (buff1[i]!=buff2[i]) return 1;
	}
}

chkpageattr(p)
struct page *p;
{
	int i,j,cc=0;

	for (i=0;i<strlen(p->page);i++) {
		if (strncmp(page_compositor,&p->page[i],strlen(page_compositor))==0) {
			p->attr[cc]=pattr[cc];
			cc++;
			i+=strlen(page_compositor)-1;
		}
		else {
ATTRLOOP:
			if (!((p->page[i]>='0' && p->page[i]<='9') || (p->page[i]>='A' && p->page[i]<='Z') || (p->page[i]>='a' && p->page[i]<='z'))) {
				p->attr[cc]= -1;
				if (cc<2) p->attr[++cc]= -1;
				return;
			}
			switch(page_precedence[pattr[cc]]) {
			case 'r':
				if (strchr("ivxlcdm",p->page[i])==NULL) {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'R':
				if (strchr("IVXLCDM",p->page[i])==NULL) {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'n':
				if (p->page[i]<'0' || p->page[i]>'9') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'a':
				if (p->page[i]<'a' || p->page[i]>'z') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			case 'A':
				if (p->page[i]<'A' || p->page[i]>'Z') {
					pattr[cc]++;
					for (j=cc+1;j<3;j++) pattr[j]=0;
					goto ATTRLOOP;
				}
				break;
			default:
				break;
			}
		}
	}
	p->attr[cc]=pattr[cc];
	if (cc<2) p->attr[++cc]= -1;
}

char *mfgets(buf,byte,fp)
char *buf;
int byte;
FILE *fp;
{
	int i,cc,cc2,cc3,jflag=0;

	for (i=0;i<byte;i++) {
		cc=fgetc(fp);
		if (cc==EOF) {
			if (i==0) return NULL;
			else break;
		}
		else if (cc=='\n') {
			buf[i++]=cc;
			break;
		}
		else if (cc>=0x80) {
			cc2=fgetc(fp);
			if (kanji==Sjis) {
				cc=SJIStoJIS((cc<<8)+cc2)+0x8080;
			}
			else {
				cc=(cc<<8)+cc2;
			}
			buf[i++]=(cc>>8)&0xff;
			buf[i]=cc&0xff;
		}
		else if (cc==0x1b) {
			cc2=fgetc(fp);
			cc3=fgetc(fp);
			if (cc2==0x24 && (cc3==0x42 || cc3==0x40))
				jflag++;
			else if (cc2==0x28 && (cc3==0x42 || cc3==0x4a))
				if (jflag>0) jflag--;
			else {
				buf[i++]=cc;
				buf[i++]=cc2;
				buf[i++]=cc3;
			}
			i--;
		}
		else if (jflag) {
			buf[i]=cc+0x80;
		}
		else buf[i]=cc;
	}
	buf[i]='\0';
	return buf;
}
