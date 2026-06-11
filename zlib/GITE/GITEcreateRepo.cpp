// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEcreateRepo
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 29 March 2023 - 06:43PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GITE/GITEazureDevops.h>
#include <GITE/GITEbitbucket.h>
#include <GITE/GITEcreateRepo.h>
#include <GITE/GITEgithub.h>
#include <GITE/GITEgitlab.h>
#include <GSV/GSVconfig.h>
#include <GSV/GSVmap.h>
COL_LOG_MODULE;

bool GITEvalidateInputParameters(const COLstring& Owner, const COLstring& Repository, const GSVitem& Item, COLstring* pError){
   COL_FUNCTION(GITEvalidateInputParameters);
   bool IsAzure          = (Item.m_Type == GIT_azuredev_cloud || Item.m_Type == GIT_azuredev_server);
   bool IsBitBucketLocal = Item.m_Type == GIT_bitbucket_server;
   bool IsGitlab         = (Item.m_Type == GIT_gitlab_cloud || Item.m_Type == GIT_gitlab_server);
   bool Success          = true;
   if( (!IsAzure && !IsBitBucketLocal) && COLstringContains(Owner, "/")){ // For Azure, the owner is <organization>/<project>. For Bitbucket Local Server, the owner is scm/<project id>                        // For local Bitbucket, owner is scm/<ProjectId>
      *pError = "Owner contains invalid \"/\" character";
      Success = false;
   }
   if(!IsGitlab && COLstringContains(Repository,"/")){
      *pError = "Repository contains invalid \"/\" character";
      Success = false;
   } 
   if( (!IsAzure && !IsBitBucketLocal) && 
      COLstringContains(Owner, "/")   && 
      COLstringContains(Repository, "/")) {
      *pError = "Owner and Repository contain invalid \"/\" character";
   }
   return Success;
}

bool GITEcreateRepo(const COLstring& Username, const COLstring& Owner, const COLstring& Repository, COLstring* pGitUrl,
						  COLstring* pError) {
   COL_FUNCTION(GITEcreateRepo);
   COL_VAR2(Owner, Repository);
   GSVconfig Config;
   GSVconfigLoadUser(Username, &Config);
	if(!GSVmapExists(Config.m_ServerUrl)) {
		*pError = Config.m_ServerUrl + " not found in server map";
		return false;
	}
	const GSVitem Item = GSVmapItem(Config.m_ServerUrl);
   if (!GITEvalidateInputParameters(Owner, Repository, Item, pError)){ return false; }
   COLstring SshUrl;
   GITUmakeSshUrl(Config.m_ServerUrl, Owner, Repository, pGitUrl);
   COL_VAR(*pGitUrl);
   try{ // TODO - instead of manually creating the strings, we should be able to get them from the responses from each host.
      switch (Item.m_Type){
         case GIT_azuredev_cloud:
         case GIT_azuredev_server:
            GITEazureRepoNew(Config, Item.m_Secure, Owner, Repository, &SshUrl);
            return true;
         case GIT_bitbucket_cloud:
         case GIT_bitbucket_server:
            GITEbitbucketRepoNew(Config, Owner, Repository);
            return true;
         case GIT_gitlab_cloud:
         case GIT_gitlab_server:
				GITEgitlabRepoNew(Config, Owner, Repository);
            return true;
         case GIT_github_cloud:
         case GIT_github_server:
            GITEgithubRepoNew(Config, Owner, Repository);
            return true;
         default:
            return false;
      }
   } catch (const COLerror& Error){
      *pError = Error.description();
      return false;
   }
}
