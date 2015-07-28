//  Copyright 2013 Google Inc. All Rights Reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <malloc.h>

const long long max_size = 100;         // max length of strings
const long long N = 500;                // number of closest words that will be shown
const long long max_w = 50;              // max length of vocabulary entries
const float Thres = 0.5;                 //Nguong loc cac quan he

//Khai bao cac kieu du  lieu moi dung de xay dung do thi cho chuong trinh

//Luu tru dinh ke
struct Ke {
    long long position; //vi tri cua dinh trong danh sach tu vung
    float distance; //gia tri cosine distance giua node goc va not ke dang xet
    struct Ke *nextNode; //Con tro tro den dinh khac
};
//Luu tru cap dinh tao voi dinh dang xet mot  tam giac
struct Cap{
    long long minNode;//Giu dinh co vi tri nho trong cap dinh
    long long maxNode;//Giu dinh co vi tri lon trong cap dinh
    struct Cap *nextNode; //Con tro tro den dinh khac
    
};

struct Dinh{
    struct Ke *keList; // Con tro tro den danh sach cac dinh ke
    struct Cap *capList;//Con tro tro den danh sach cac cap dinh tao voi dinh dang xet tam giac
    long  numCap;//So cap tao voi dinh dang xet mot tam giac
    char tu[100]; //Tu duoc luu tru trong node nay
};

int main(int argc, char **argv) {
  FILE *f , *file; // file bin dau vao
  char st1[max_size];
  char *bestw[N]; //Mang chua cac tu co quan he manh nhat voi tu vung nhap vao
  char file_name[max_size], st[100][max_size];
  float dist, len, bestd[N], vec[max_size];
  long long words, size, i, a, b, c, d, cn, bi[100];//Words la so tu vung trong file dau vao
  char ch;
  float *M;
  char *vocab;
  float debug;
  char line[256]; 
  long position[N]; //Mang luu vi tri cua tu co lien he cao voi tu dang xet
  struct Dinh Node[2235];  //Mảng lưu trữ các đỉnh của đồ thị.Số đỉnh được tính trước từ file từ v
  long curPos; // Luu tru thu tu cua tu dang xet
  struct Ke *curKe; //Con tro trỏ vao node ke dang xet ,
  struct Ke *headerKe;//con trỏ trỏ vào node đầu tiên của danh sách
  struct Ke *tempKe;  //Trỏ đến vùng nhớ tạm thời của của node Kề
  
  struct Cap *curCap; //Con trỏ trỏ vào node Cap đang xét , con trỏ trỏ vào node đầu tiến cảu 
  struct Cap *headerCap; //Lưu trữ node đầu tiên của danh sách
  struct Cap *tempCap;  //Trỏ đến vùng nhớ tạm thời của node Cap
  //FIle từ vựng
   file = fopen("vocab.txt", "r");
  //File từ đã được  luyện
  strcpy(file_name,"restaurant.bin");
  f = fopen(file_name, "rb");
  if (f == NULL) {
    printf("Input file not found\n");
    return -1;
  }
  fscanf(f, "%lld", &words); //gan so tu vung co trong file cho bien words
  fscanf(f, "%lld", &size);
    
  debug = (long long)words * max_w * sizeof(char);
  vocab = (char *)malloc((long long)words * max_w * sizeof(char));
  for (a = 0; a < N; a++) bestw[a] = (char *)malloc(max_size * sizeof(char)); //Khoi tao bo nho cho nhung tu tim duoc
  M = (float *)malloc((long long)words * (long long)size * sizeof(float));
  if (M == NULL) {
    printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)words * size * sizeof(float) / 1048576, words, size);
    return -1;
  }
  for (b = 0; b < words; b++) {
    a = 0;
    while (1) {
      vocab[b * max_w + a] = fgetc(f);
      if (feof(f) || (vocab[b * max_w + a] == ' ')) break;
      if ((a < max_w) && (vocab[b * max_w + a] != '\n')) a++;
    }
    
    vocab[b * max_w + a] = 0;
    for (a = 0; a < size; a++) fread(&M[a + b * size], sizeof(float), 1, f);
    len = 0;
    for (a = 0; a < size; a++) len += M[a + b * size] * M[a + b * size];
    len = sqrt(len);
    for (a = 0; a < size; a++) M[a + b * size] /= len;
  }
  fclose(f);
  while (fgets(line, sizeof(line), file)) {
//    for (a = 0; a < N; a++) bestd[a] = 0;
//    for (a = 0; a < N; a++) bestw[a][0] = 0;
//    printf("Enter word or sentence (EXIT to break): ");
//    a = 0;
//    while (1) {
//      st1[a] = fgetc(stdin);
//      if ((st1[a] == '\n') || (a >= max_size - 1)) {
//        st1[a] = 0;
//        break;
//      }
//      a++;
//    }
//    if (!strcmp(st1, "EXIT")) break;
      
    for( i=0; i< 256; i++){
        if((int)line[i] == 32){
            line[i]='\0';
            break;
        }
    }
    strcpy(st1,line);
    cn = 0;
    b = 0;
    c = 0;
    while (1) {
      st[cn][b] = st1[c];
      b++;
      c++;
      st[cn][b] = 0;
      if (st1[c] == 0) break;
      if (st1[c] == ' ') {
        cn++;
        b = 0;
        c++;
      }
    }
    cn++;
    for (a = 0; a < cn; a++) {
      for (b = 0; b < words; b++) if (!strcmp(&vocab[b * max_w], st[a])) break;
      if (b == words) b = -1;
      bi[a] = b;
     // printf("\nWord: %s  Position in vocabulary: %lld\n", st[a], bi[a]);
      if (b == -1) {
        printf("Out of dictionary word!\n");
        break;
      }
    }
    if (b == -1) continue;
    //Xây dưng node của đồ thi
    curPos = b;
    strcpy(Node[curPos].tu,st1);

    
    
   // printf("\n                                              Word       Cosine distance\n------------------------------------------------------------------------\n");
    for (a = 0; a < size; a++) vec[a] = 0;
    for (b = 0; b < cn; b++) {
      if (bi[b] == -1) continue;
      for (a = 0; a < size; a++) vec[a] += M[a + bi[b] * size];
    }
    len = 0;
    for (a = 0; a < size; a++) len += vec[a] * vec[a];
    len = sqrt(len);
    for (a = 0; a < size; a++) vec[a] /= len;
    for (a = 0; a < N; a++) bestd[a] = -1;
    for (a = 0; a < N; a++) bestw[a][0] = 0;
    for (c = 0; c < words; c++) {
      a = 0;
      for (b = 0; b < cn; b++) if (bi[b] == c) a = 1;
      if (a == 1) continue;
      dist = 0;
      for (a = 0; a < size; a++) dist += vec[a] * M[a + c * size];
      for (a = 0; a < N; a++) {
        if (dist > bestd[a]) {
          for (d = N - 1; d > a; d--) {
            bestd[d] = bestd[d - 1];
            strcpy(bestw[d], bestw[d - 1]);
            position[d] = position[d-1];
          }
          bestd[a] = dist;
          strcpy(bestw[a], &vocab[c * max_w]);
          position[a] = c;
          break;
        }
      }
    }
    
//    for (a = 0; a < N; a++){
//        if(bestd[a] > Thres){
//          printf("%50s\t\t%f\n",bestw[a], bestd[a]);
//        }
//    }
        
    for (a = 0; a < N; a++){
        if(bestd[a] > Thres){
           // printf("%ld,%50s\t\t%f\n",position[a],bestw[a], bestd[a]);
            //Viet code xay dung do thi o day
            tempKe = (struct Ke *)malloc(sizeof(struct Ke));
            tempKe->distance = bestd[a];
            tempKe->nextNode = NULL;
            tempKe->position = position[a];
            
            if(a == 0){
                headerKe = tempKe;
                
            }else{
                curKe->nextNode = tempKe;
            }
            curKe = tempKe;
            
        }
    }
    //Gán danh sách vừa tạo cho node đang xét
    Node[curPos].keList = headerKe;
    for(a =curPos; a<= curPos; a++){
        printf("%s::\n",Node[a].tu);
        curKe = Node[a].keList;
        while(curKe!=NULL){
            printf("%s  ",&vocab[curKe->position *  max_w]);
            curKe = curKe->nextNode;
        }
        printf("\n");
    }
    b = 0;
  }
  return 0;
}
void buildTree(){
  
}
