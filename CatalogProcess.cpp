CString DetailsProcess(CString input_file_name,CString link_start,CString link_stop,CString nex_page_start,CString nex_page_stop)
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
    CString next_page;
    next_page.Format(_T("%d"),atoi(input_file_name)+1);
    AsyncJob* job=new AsyncJob;
    job->type=GET_PAGE;
    job->address=address;
    job->data=next_page;
    job_queue->Add(job);
  }
  
  //find all links to details
  bool find=true;
  int link_num=0;
  while(find)
  {
    start_pos=data.Find(link_start);
    stop_pos=data.Find(link_stop);
    if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
    {
      CString address=data.Mid(start_pos,stop_pos-start_pos);
      data=data.Right(data.GetLength()-stop_pos);
      
      //start job to get details page
      CString details_page;
      details_page.Format(_T("_%d!%d"),atoi(input_file_name),++link_num);
      AsyncJob* job=new AsyncJob;
      job->type=GET_PAGE;
      job->address=address;
      job->data=details_page;
      job_queue->Add(job);
    }
    else find=false;
  }
  
  fclose(input_file);
  
  return result;
}
