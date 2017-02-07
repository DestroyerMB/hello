void DetailsProcess(CString input_file_name,CString link_start,CString link_stop,CString nex_page_start,CString nex_page_stop)
{
  
  FILE *input_file = _wfopen(connectFilePath, _T("rb")); 
  if (!input_file) return; 
  
  CString data,result;
  
  fseek(input_file, 0, SEEK_END); 
  lSize = ftell(input_file); 
  rewind(input_file); 

  LPBYTE lpContent = (LPBYTE) malloc(lSize); 
  fread(input_file, 1, lSize, fConnect);
  fclose(lpContent);
  data = lpContent;
  free(lpContent); 
  
  //find next page
  int start_pos=data.Find(next_page_start);
  int stop_pos=data.Find(next_page_stop);
  if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
  {
    CString address=data.Mid(start_pos,stop_pos-start_pos);
    //start job to get next page
    !!!
  }
  
  //find all links to details
  bool find=true;
  while(find)
  {
    start_pos=data.Find(link_start);
    stop_pos=data.Find(link_stop);
    if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
    {
      CString address=data.Mid(start_pos,stop_pos-start_pos);
      data=data.Right(data.GetLength()-stop_pos);
      //start job to get details page
      !!!
    }
    else find=false;
  }
  
  FILE *otput_file=fopen(output_file_name,"a");
  if(!output_file) otput_file=fopen(output_file_name,"w");
  if(!output_file) return;
  fputs(output_fie,result,1);
  
  fclose(input_file);
  fclose(output_file);
}
