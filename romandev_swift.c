//
//  main.c
//  sktroman
//
//  Created by 岡崎康浩 on 2022/07/16.
//
#include "devnag.h"

unsigned char wide_to_lower( unsigned char in_char )
{
    if(char_flag == 0 && 'A' <= in_char && in_char <= 'Z'){
        char_flag = 0;
        return (in_char +('a'-'A'));
    }
    if(char_flag == 0 && (in_char == 0xc4 || in_char == 0xc5)){
        char_flag = 1;
        return in_char;
    }
    if(char_flag == 0 && in_char == 0xe1){
        char_flag = 4;
        return in_char;
    }
    if(char_flag == 4 && (in_char == 0xb9 || in_char == 0xb8)){
        char_flag -= 2;
        return in_char;
    }
    if(char_flag == 2){
        switch(in_char){
            case     0x9a:
            case     0x9c:
            case     0xb6:
            case     0xb8:
            case     0x84:
            case     0xac:
            case     0x8c:
            case     0xa2:
            case     0xa4:
            case     0x82:
            case     0x86:
                in_char += 1;
                break;
            default:break;
        }
        char_flag = 0;
        return in_char;
    }
    if(char_flag == 1){
        switch(in_char){
            case 0x80:
            case 0xaa:
            case 0xb0:
            case 0x9a:
            case 0xac:
                in_char += 1;
                break;
            default: break;
        }
        char_flag = 0;
        return in_char;
    }
    return in_char;
}
int one_character(unsigned char str)
{
    if(str < 0x80) return 1;
    if(str < 0xE0) return 2;
    if(str < 0xF0) return 3;
    return 4;
}
/*
 make_dvcode can be used for 1byte letter only.
 */
dvcode *make_dvcode(unsigned char *s, unsigned char *p, int mode)
{
    int j;
    dvcode  *tmp_dvcode;
    tmp_dvcode = malloc(sizeof(dvcode));
    if(tmp_dvcode == NULL){
        printf("The memory for Devanagari code is not enough!\n");
        return NULL;
    }
    tmp_dvcode->internal_sign = s[0];
    strcpy((char*)(tmp_dvcode->dv),(char*)s);
    strcpy((char*)(tmp_dvcode->rm),(char*)s);
    for(j=1;j<8;j++)tmp_dvcode->rm[j] = '\0';
    tmp_dvcode->dvmode = mode;
    tmp_dvcode->rm_length = 1;
    tmp_dvcode->dv_length = 1;
    for(j=0;j<6;j++)tmp_dvcode->byte[j] = NULL;
    tmp_dvcode->allograph = NULL;
    tmp_dvcode->comm = p;
    return tmp_dvcode;
}

int read_list( const char* cdir_name )
{
    FILE* def_file = NULL;
    unsigned char code;
    int     rf, i, ci,j;
    struct dvcode *tmp_list;
    //struct dvcode *next_list;
//    char def_file_name[64];
//    char full_def_file_name[MAX_DIR];
//    strcpy(def_file_name,"define_dev_list.tbl");
//    sprintf(full_def_file_name, "%s/%s",cdir_name, def_file_name);
    def_file = fopen((char*)cdir_name,"r");//fopen(full_def_file_name,"r");
    if(def_file == NULL){
       // printf("path = %s\n", full_def_file_name);
        printf("errno = %d\n", errno);
        printf("error = %s\n", strerror(errno));
        printf("Cannot open define_dev_list\n");
        printf("Plese place the file \"define_dev_list.tbl\" on your current directory\n");
        printf("where the input_file is placed\n");
        return 1;
    }
    rf = 0;
    i = 0;
    ci = 0;
    //next_list = NULL;
    tmp_list = NULL;
    while((signed char)(code = getc(def_file))!= EOF){
        if(code == '%') {
            rf = 0;
            continue;
        }
        if(code == '\n')
        {
            rf = 0;
            i = 0;
            continue;
        }
        if(code == '!' && rf == 0)
        {
            rf = 5;
            i = 0;
            tmp_list = malloc(sizeof(dvcode));
            if(tmp_list == NULL){
                printf("The memory for Devanagari code is not enough!\n");
                return 1;
            }
            devnag[ci] = tmp_list;
            ci++;
            
            code = getc(def_file);
            switch(code){
                case '0': tmp_list->dvmode = 0;
                    break;
                case '1': tmp_list->dvmode = 1;
                    break;
                case '2': tmp_list->dvmode = 2;
                    break;
                case '3': tmp_list->dvmode = 3;
                    break;
                case '4': tmp_list->dvmode = 4;
                    break;
                case '5': tmp_list->dvmode = 5;
                    break;
                case '6': tmp_list->dvmode = 6;
                    break;
                case '7': tmp_list->dvmode = 7;
                    break;
                case '8': tmp_list->dvmode = 8;
                    break;
                case '9': tmp_list->dvmode = 9;
                    break;
                default:
                    printf("The format of Devanagari list is mistaken! dvmode = %c\n",code);
                    return 1;
            }
            for(j=0;j<6;j++) tmp_list->byte[j] = NULL;
            tmp_list->allograph = NULL;
            tmp_list->comm = NULL;
            tmp_list->read = 0;
            continue;
        }
        if(rf == 5 && code == '<'){
            i = 0;
            rf = 4;
            continue;
        }
        if(code == '?' && rf == 4){
            tmp_list->dv[i] = '\0';
            tmp_list->dv_length = i;
            i = 0;
            rf = 3;
            continue;
        }
        if(code == '>' && rf == 3)
        {
            tmp_list->rm[i] = '\0';
            tmp_list->rm_length = i;
            i++;
            while(i < 8){
                tmp_list->rm[i] = '\0';
                i++;
            }
            code = getc(def_file);
            tmp_list->internal_sign = code;
            rf = 1;
            continue;
        }
        if(rf == 4){
            tmp_list->dv[i] = code;
            i++;
            continue;
        }
        if(rf == 3){
            tmp_list->rm[i] = code;
            i++;
            continue;
        }
        if(rf == 0) continue;
    }
    tmp_list = make_dvcode((unsigned char*)"?",NULL,9);
    devnag[ci] = tmp_list;
    ci++;
    tmp_list = make_dvcode((unsigned char*)"\n",NULL,0);
    devnag[ci] = tmp_list;
    ci++;
    tmp_list = make_dvcode((unsigned char*)"<",NULL,7);
    devnag[ci] = tmp_list;
    ci++;
    tmp_list = make_dvcode((unsigned char*)"<",NULL,7);
    devnag[ci] = tmp_list;
    ci++;
    tmp_list = make_dvcode((unsigned char*)"!",NULL,9);
    devnag[ci] = tmp_list;
    ci++;
    tmp_list = make_dvcode((unsigned char*)"\t",NULL,8);
    devnag[ci] = tmp_list;
    ci++;
    devnag[ci] = NULL;
    code_num_real = ci;
    fclose(def_file);
    return 0;
}
/*
Preparation for making research tree. Sort the conversion table in lexicographical order. The return value is the number of entries in the conversion table
 */
int   maintree( void )
{
    int     code_index;
    int     i,j;
    code_index = 0;
    for(i=0;i<code_num_real;i++){
        for(j=0;j<code_num_real;j++){
            if(devnag[j]->read == 0 && memcmp(devnag[code_index]->rm,devnag[j]->rm,7) < 0){
                code_index = j;
            }
        }
        temp_order[i] = code_index;
        devnag[code_index]->read = 1;
        for(j=0;j<code_num_real;j++){
            if(devnag[j]->read == 0) break;
        }
        code_index = j;
    }
    return i;
}
int     subtree(int start, int end, int d)
{
    int                 i, temp_i,temp_e,temp_d,temp_deepest,node[7];
    if(start == end ) return end;
    if(devnag[temp_order[start]]->rm_length < d) return start;
    node[0] = end;
    for(i=1;i<7;i++) node[i] = 0;
    temp_i = start + 1;
    temp_e = end;
    temp_d = d;
    temp_deepest = devnag[temp_order[start]]->rm_length;
    while(start+1 < end){
        temp_deepest = devnag[temp_order[start]]->rm_length;
        while(temp_d < temp_deepest){
            temp_i = start + 1;
            while(temp_i < temp_e)
            {
                if(devnag[temp_order[start]]->rm[temp_d]!= devnag[temp_order[temp_i]]->rm[temp_d]){
                    devnag[temp_order[start]]->byte[temp_d] = devnag[temp_order[temp_i]];
                    node[temp_d] = temp_i;
                    //temp_deepest--;
                    break;// i++;
                }
                temp_i++;
            }
            temp_d++;
        }
        if(temp_i == end) return temp_i;
        temp_d--;
        while(temp_d > -1 && node[temp_d] == 0)temp_d--;
        //start = node[temp_d];
        while(temp_d > -1 && devnag[temp_order[start]]->rm_length < temp_d){
            temp_d--;}
            start = node[temp_d];
    }
    return temp_i;
}

/* Creating a research tree. The return value is a pointer to the root entry. */

dvcode*     maketree( void )
{
    static int i,result;
    unsigned char   b1;
    static int         st,depth;
    result = maintree();
    depth = 0;
    b1 = devnag[temp_order[0]]->rm[depth];
    st = 0;
    for(i=0;i<code_num_real;i++)
    {
        if(devnag[temp_order[i]]->dvmode == 3 && devnag[temp_order[i]]->internal_sign == devnag[temp_order[i+1]]->internal_sign) devnag[temp_order[i]]->allograph = devnag[temp_order[i+1]];
        if(devnag[temp_order[i]]->rm[0] == b1) continue;
        devnag[temp_order[st]]->byte[0] = devnag[temp_order[i]];
        subtree(st,i,1);
        st = i;
        b1 = devnag[temp_order[i]]->rm[0];
    }
//   for(i=0;i<code_num_real;i++)
//        printf("rm:(%03d)->%03d: %s:[%02x:%02x:%02x:%02x]: addr1 = %s :addr2 = %s :addr3 = %s :addr4 = %s\n",i,temp_order[i],devnag[temp_order[i]]->rm,devnag[temp_order[i]]->rm[0],devnag[temp_order[i]]->rm[1],devnag[temp_order[i]]->rm[2],devnag[temp_order[i]]->rm[3],devnag[temp_order[i]]->byte[0]==NULL?"NULL":(char*)(devnag[temp_order[i]]->byte[0]->rm),devnag[temp_order[i]]->byte[1]==NULL?"NULL":(char*)(devnag[temp_order[i]]->byte[1]->rm),devnag[temp_order[i]]->byte[2]==NULL?"NULL":(char*)(devnag[temp_order[i]]->byte[2]->rm), devnag[temp_order[i]]->byte[3]==NULL?"NULL":(char*)(devnag[temp_order[i]]->byte[3]->rm));
    return devnag[temp_order[0]];
}

/*
 pre_search:
 */
dvcode*     pre_search( unsigned char lit_1){
    dvcode  *temp_dvcode;
    temp_dvcode = root_dvcode;
    while(temp_dvcode != NULL){
        if(temp_dvcode->rm[0] == lit_1) return temp_dvcode;
        temp_dvcode = temp_dvcode->byte[0];
    }
    return NULL;
}
/*
 deeper_search:
 */
dvcode*     deeper_search( unsigned char *lit_seq, dvcode *mother, int d)
{
    if(mother == NULL) return NULL;
    if(d >= mother->rm_length) return mother;
    if(*(lit_seq+d)== mother->rm[d]){
        d++;
        return(deeper_search(lit_seq, mother,d));
    }
    else{
        return(deeper_search(lit_seq,mother->byte[d],d));
    }
}

int read_buf(int ip)
{
    dvcode  *temp_list;
    int     temp_ip, i, len;
    unsigned char   temp_char;
    unsigned char   temp_seq[8];
   // unsigned char*   comm_seq;
    temp_ip = ip;
    temp_char = ori_buf[temp_ip];
    temp_list = pre_search(temp_char);
    if(temp_list == NULL){
        len = one_character(temp_char);
        for(i=0;i<len;i++) temp_seq[i] = ori_buf[temp_ip+i];
        temp_seq[i] = '\0';
        temp_list = make_dvcode(temp_seq, (unsigned char*)"", 10+len);
        if(temp_list != NULL){
            internal_buf[inbuf] = temp_list;
            inbuf++;
        }
        else{
            printf("The memory for reading buffer is not enough!\n");
        }
        printf("%s is not romanized Sanskrit\n", temp_seq);
        return temp_ip + i;
    }
    len = temp_list->rm_length;
    for(i=0;i<len;i++) temp_seq[i] = ori_buf[temp_ip+i];
    temp_seq[len] = '\0';
    temp_list = deeper_search(temp_seq, temp_list, 1);
    if(temp_list == NULL){
        len = one_character(temp_char);
        for(i=0; i < len; i++) temp_seq[i] = ori_buf[temp_ip+i];
        temp_seq[i+1] = '\0';
        temp_list = make_dvcode(temp_seq, (unsigned char*)"", 10+len);
        if(temp_list != NULL){
            internal_buf[inbuf] = temp_list;
            inbuf++;
        }
        else{
            printf("The memory for reading buffer is not enough!\n");
        }
        printf("%s is not romanized Sanskrit\n", temp_seq);
        return temp_ip + i;
    }
    internal_buf[inbuf] = temp_list;
    inbuf++;
    temp_ip = ip + temp_list->rm_length;
    return temp_ip;
}

int write_devnag(int i_ip)
{
    int     temp_mode, tmp_i_ip;
    dvcode  *tmp_vowel;
    temp_mode = internal_buf[i_ip]->dvmode;
    if(temp_mode > 10){
        strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dvmode - 10);
        write_ip = write_ip + (internal_buf[i_ip]->dvmode - 10);
     //   free(internal_buf[i_ip]->comm);
        free(internal_buf[i_ip]);
        return i_ip + 1;
    }
    switch(temp_mode){
        case INDEPEND: strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
            write_ip = write_ip + internal_buf[i_ip]->dv_length;
            return i_ip + 1;
            break;
        case CONSONANT:
            if(internal_buf[i_ip + 1]==NULL) return 0;
            switch(internal_buf[i_ip + 1]->dvmode){
            case SHORT_A:
                strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                write_ip = write_ip + internal_buf[i_ip]->dv_length;
                return i_ip + 2;
                break;
            case VOWEL_DE:
                strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                write_ip = write_ip + internal_buf[i_ip]->dv_length;
                tmp_vowel = internal_buf[i_ip + 1]->allograph;
                strncpy((char*)write_ip,(char*)(tmp_vowel->dv),tmp_vowel->dv_length);
                write_ip = write_ip + tmp_vowel->dv_length;
                return i_ip + 2;
                break;
            case SPACE: tmp_i_ip = 1;
                while(internal_buf[i_ip+tmp_i_ip] != NULL && internal_buf[i_ip+tmp_i_ip]->dvmode == SPACE) tmp_i_ip++;
                    if(internal_buf[i_ip+tmp_i_ip] == NULL) break;
                switch(internal_buf[i_ip+tmp_i_ip]->dvmode){
                    case SHORT_A:
                        strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                        write_ip = write_ip + internal_buf[i_ip]->dv_length;
                        return i_ip + tmp_i_ip + 1;
                        break;
                    case VOWEL_DE:
                        strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                        write_ip = write_ip + internal_buf[i_ip]->dv_length;
                        tmp_vowel = internal_buf[i_ip + tmp_i_ip]->allograph;
                        strncpy((char*)write_ip,(char*)(tmp_vowel->dv),tmp_vowel->dv_length);
                        write_ip = write_ip + tmp_vowel->dv_length;
                        return i_ip + tmp_i_ip + 1;
                        break;
                    case CONSONANT:
                        strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                        write_ip = write_ip + internal_buf[i_ip]->dv_length;
                        strncpy((char*)write_ip,(char*)VIRAMA,3);
                        write_ip = write_ip + 3;
                        return i_ip + tmp_i_ip;
                        break;
                    default:
                        strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                        write_ip = write_ip + internal_buf[i_ip]->dv_length;
                        strncpy((char*)write_ip,(char*)VIRAMA,3);
                        write_ip = write_ip + 3;
                        return i_ip + 1;
                        break;
                }
            default:
                strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
                write_ip = write_ip + internal_buf[i_ip]->dv_length;
                strncpy((char*)write_ip,(char*)VIRAMA,3);
                write_ip = write_ip + 3;
                return i_ip + 1;
                break;
        }
        default:
            strncpy((char*)write_ip,(char*)(internal_buf[i_ip]->dv),internal_buf[i_ip]->dv_length);
            write_ip = write_ip + internal_buf[i_ip]->dv_length;
            return i_ip = i_ip + 1;
    }
}

int prepare(const char* path)
{
    int ret;
    ret = read_list(path);
    if(ret == 1) return ret;
    root_dvcode = maketree();
    return ret;
}


unsigned char *convert_text(const unsigned char *src)
{
    size_t pos = 0;
    size_t src_len;
    int lit, temp_char;
    int pt = 0;
    int con_flag = 0;
    unsigned char *dst;
    unsigned char *dst_pos;

    if (src == NULL) return NULL;

    src_len = strlen((const char *)src);

    dst = malloc(src_len * 3 + 1);
    if (dst == NULL) return NULL;

    dst[0] = '\0';
    dst_pos = dst;

    while ((lit = src[pos]) != '\0') {
        pos++;

        if (lit == '|' || lit == '/') {
            temp_char = lit;
            if (src[pos] == temp_char) {
                lit = src[pos];
                pos++;
                ori_buf[pt] = (unsigned char)lit;
                ori_buf[pt + 1] = (unsigned char)lit;
                ori_buf[pt + 2] = '\0';
            } else {
                ori_buf[pt] = (unsigned char)temp_char;
                ori_buf[pt + 1] = '\0';
            }
            pt = 0;
            con_flag = 1;
        }
        else if (lit == '.' || lit == '?' || lit == '!') {
            ori_buf[pt] = (unsigned char)lit;
            ori_buf[pt + 1] = '\0';
            pt = 0;
            con_flag = 1;
        }
        else if (con_flag == 0) {
            ori_buf[pt] = wide_to_lower((unsigned char)lit);
            pt++;
            continue;
        }

        read_ip = 0;
        inbuf = 0;
        while (ori_buf[read_ip] != '\0') {
            read_ip = read_buf(read_ip);
        }
        internal_buf[inbuf] = NULL;

        inbuf = 0;
        write_ip = conv_buf;
        while (internal_buf[inbuf] != NULL) {
            inbuf = write_devnag(inbuf);
        }
        *write_ip = '\0';
        strcpy((char *)dst_pos, (const char *)conv_buf);
        dst_pos += strlen((const char *)conv_buf);

        con_flag = 0;
    }
    
    if(pt > 0){
        ori_buf[pt] ='\0';
        read_ip = 0;
        inbuf = 0;
        while (ori_buf[read_ip] != '\0') {
            read_ip = read_buf(read_ip);
        }
        internal_buf[inbuf] = NULL;
        inbuf = 0;
        write_ip = conv_buf;
        while (internal_buf[inbuf] != NULL) {
            inbuf = write_devnag(inbuf);
        }
        *write_ip = '\0';

        strcpy((char *)dst_pos, (const char *)conv_buf);
        dst_pos += strlen((const char *)conv_buf);
        *dst_pos = '\0';
    }

    *dst_pos = '\0';
    return dst;
}

void free_text( unsigned char *p)
{
    free(p);
}
/*
int main(int argc, char *argv[]) {
    // insert code here...
    int         result,pt, byte_num, read_ip;
    unsigned char lit;
    char input_file_name[128];
    char output_file_name[128];
    char opt, temp_char;
    FILE* i_file = NULL;
    o_file = NULL;
    line = 0;
    page_f = 0;
    line_f = 0;
    getcwd(cdir_name,MAX_DIR);
    printf("This Program will reduce some romanized text into some devanagari document\n");
    printf("whose devanagari is encoded by utf-8.\n");
    printf("rm2dn\n");
    printf("The copyright of this program belongs to Yasuhiro Okazaki.\n");
    printf("kappakappa.yasu@gmail.com   2022.7.18\n\n\n");
    
    if(argc == 1){
        printf("Please write the input file name. For example\n");
        printf("dn2rm xxx.txt\n");
        printf("This program is terminated\n");
        return 0;
    }
    while((opt = getopt(argc, argv, "pl"))!= -1){
        switch(opt){
            case 'p':
            case 'P':
                page_f = 1;
                break;
            case 'l':
            case 'L':
                line_f = 1;
                break;
            default:
                printf("Unknown Option Switch:%c", opt);
                break;
        }
    }
    while (optind < argc) {
        strcpy(input_file_name, argv[optind]);
        optind++;
    }
    
    strcpy(output_file_name, input_file_name);
    if((strchr(input_file_name,'.'))== NULL){
        strcat(input_file_name,".txt");
    }
    else
    {
        strtok(output_file_name, ".");
    }
    strcat(output_file_name,"_dn.txt");
    printf("Devanagari file: %s\n",output_file_name);
    printf("Original Romanized file: %s\n",input_file_name);
    result = read_list();
    if(result != 0) return 1;
    root_dvcode = maketree();
    i_file = fopen(input_file_name, "r");
    if(i_file == NULL)
    {
        printf("Cannot open input file %s!\n", input_file_name);
        return 1;
    }
    o_file = fopen(output_file_name, "w");
    if(o_file == NULL)
    {
        printf("Cannot open output file!\n");
        return 1;
    }
    pt = 0;
    byte_num = 0;
    con_flag = 0;
    char_flag = 0;
    while((signed char)(lit = getc(i_file))!= EOF){
        if(lit == '|' || lit == '/'){
            temp_char = lit;
            if((signed char)(lit = getc(i_file)) == temp_char){
                ori_buf[pt] = (unsigned char)lit;
                ori_buf[pt+1] = (unsigned char)lit;
                ori_buf[pt+2] = '\0';
            }
            else{
                ori_buf[pt] = (unsigned char)temp_char;
                ori_buf[pt+1] = '\0';
                ungetc((int)lit,i_file);
            }
            pt = 0;
            con_flag = 1;
        }
        if(lit == '.' || lit == '?' || lit == '!'){
            ori_buf[pt] = lit;
            ori_buf[pt+1] = '\0';
            pt = 0;
            con_flag = 1;
        }
        if(con_flag == 0){
            ori_buf[pt] = wide_to_lower((unsigned char)lit);
            pt++;
            continue;
        }
        read_ip = 0;
        inbuf = 0;
        while(ori_buf[read_ip] != '\0')
        {
            read_ip = read_buf(read_ip);
        }
        internal_buf[inbuf] = NULL;
        inbuf = 0;
        write_ip = conv_buf;
        while(internal_buf[inbuf] != NULL)
        {
            inbuf = write_devnag(inbuf);
        }
        *write_ip = '\0';
        fputs((char*)conv_buf,o_file);
        con_flag = 0;
    }
    fclose(i_file);
    fclose(o_file);
    printf("\n\nIf error message \"Non listed Letter in line...\" is returned,\n");
    printf("it means that there is any unromanizable letter in input_file.\n");
    printf("You should search token <\?\?> in output_file,\n");
    printf("and correct the corresponding letter in input_file\n");
    printf("The line numbers of output_file exactly correspond to that of input_file\n");
    printf("If error message \"Scribal Error!<AE>/<VE>\" is returned,\n");
    printf("it means that the usage of vowel(ac) sign or virama is mistaken.\n");
    printf("You should search token <AE>/<VE> in output_file,\n");
    printf("and correct the corresponding sign in input_file\n\n\n");
    printf("All process is done. Processed Line: %d\n", line);
    printf("The romanized file %s has been created\n", output_file_name);
    return 0;
}*/
