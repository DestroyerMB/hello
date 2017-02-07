  if(job->type==GET_PAGE)
  {
    if(job->data.Find('_')>=0)
      ProcessDetails(...);
    else ProcessCatalog(...);
  }
  else if(job->type==PROCESS_DETAILS)
  {
    
  }
