void DetailsProcess(CString input_file_name,CStringArray names,CStringArray starts,CStringArray stops,CString output_file_name)
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
  
  for(int phrase=0;phrase<names.GetCount();phrase++)
  {
    CString start_phrase=starts[phrase];
    CString stop_phrase=stops[phrase];
    int start_pos=data.Find(start_phrase);
    int stop_pos=data.Find(stop_phrase);
    if(start_pos>=0 && stop_pos>=0 && stop_pos>start_pos)
    {
      if(!resut.IsEmpty()) result+=_T(",");
      result+=data.Mid(start_pos,stop_pos-start_pos);
    }
  }
  result+=_T("\n");
  
  FILE *otput_file=fopen(output_file_name,"a");
  if(!output_file) otput_file=fopen(output_file_name,"w");
  if(!output_file) return;
  fputs(output_fie,result,1);
  
  fclose(input_file);
  fclose(output_file);
}
