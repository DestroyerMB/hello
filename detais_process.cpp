void DetailsProcess(CString input_file_name,CStringArray names,CStringArray starts,CStringArray stops,CString output_file_name)
{
  FILE *input_file=fopen(input_file_name,"r");
  if(!input_file) return;
  
  CString data,result;
  
  FILE *input_file = _wfopen(connectFilePath, _T("rb")); 
  if (!fConnect) 
    return; 
  fseek(fConnect, 0, SEEK_END); 
  lSize = ftell(fConnect); 
  rewind(fConnect); 

  LPBYTE lpContent = (LPBYTE) malloc(lSize); 
  fread(lpContent, 1, lSize, fConnect);
  fclose(lpContent);
  free(lpContent); 
  
  
  for(int phrase=0;phrase<names.GetCount();phrase++)
  {
    CString start_phrase=starts[phrase];
    CString stop_phrase=stops[phrase];
    int pos=()
    
  }
  result+=_T("\n");
  
  FILE *otput_file=fopen(output_file_name,"a");
  if(!output_file) otput_file=fopen(output_file_name,"w");
  if(!output_file) return;
  fputs(output_fie,result,1);
  
  fclose(input_file);
  fclose(output_file);
}
