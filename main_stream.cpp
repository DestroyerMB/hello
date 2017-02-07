  if(job->type==GET_PAGE)
  {
    if(job->data.Find('_')>=0)
      ProcessDetails(...);
    else ProcessCatalog(...);
  }
  else if(job->type==PROCESS_DETAILS)
  {
    FILE *output_file=fopen(output_file_name,"a");
    if(output_file)
    {
      fputs(output_file,job->result_data,1);
      fclose(output_file);
    }
  }
