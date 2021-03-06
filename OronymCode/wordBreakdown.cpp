#include "wordBreakdown.h"

using namespace std;


//GLOBALS for database access
sqlite3 *db;
vector< vector< string > > databaseResults;
DBType dbflag;



static int callback(void *queryterm, int nCol, char **values, char **headers){
   int i;
   vector<string> rowEntry;
   //fprintf(stderr,"===Callback for query %s===\n",(char *) queryterm);
   for(i=0; i< nCol; i++){
      //fprintf(stderr,"%s = %s\n", headers[i], values[i] ? values[i] : "NULL");
      rowEntry.push_back( values[i] );
   }
   //printf("\n");
   databaseResults.push_back(rowEntry);
   return 0;
}

bool confirmDatabaseInitialization() {
   bool allValsInitialized = true;
   if(db == NULL ) {
      connectToPhoneticDictionaryDatabase();
   }
   return allValsInitialized &= (db != NULL);
}

vector< vector<phone> > getPhoneSeqsForSampaStrs( vector<string> sampaStrings ) {
   vector< vector <phone> > sampaSyllPhrases;
   for(int i = 0; i < sampaStrings.size(); i++) {
      vector<phone> sampaSylls = parseSAMPAintoPhonemes( sampaStrings[i] );
      sampaSyllPhrases.push_back(sampaSylls);
    }
   return sampaSyllPhrases;
}

vector< vector<phone> > getPhoneSeqsForOrthoWord( string orthoWord ) {
   vector<string> sampaStrings = queryDBwithOrthoForSampaStrs( orthoWord );
   return getPhoneSeqsForSampaStrs( sampaStrings );
}

/*given an phrase of ortho words, gives all the sampa permutations
 that it could possibly be, where each inner vector is a phonetic permutation of
 the full phrase */
vector< vector<phone> > findAllPhoneSeqsForOrthoPhrase( string orthoPhrase ) {
	
	vector<string> orthoWords = strTokOnWhitespace( orthoPhrase );
	vector< vector<phone> > fullPhrasePhoneSeqs; 
   //cerr << "FIND ALL PERMUTATIONS" << endl;
   for (int i = 0; i < orthoWords.size(); i++) {
      vector<string> sampaStrings = queryDBwithOrthoForSampaStrs( orthoWords[i] );
      vector< vector<phone> > nextWordSAMPAPhoneSeqs = getPhoneSeqsForOrthoWord( orthoWords[i] );
         /*
         //DEBUG
         cerr << j <<": ";
         for ( int k = 0; k < nextWordSAMPAPhoneSeqs[j].size(); k++ ) {
            cerr<< "_" << nextWordSAMPAPhoneSeqs[j][k] << "_";
         }
         cerr << endl; 
         //END DEBUG
         */  
         if( nextWordSAMPAPhoneSeqs.empty() ) {
            cout<< "The word '"<<orthoWords[i]<<"was not found in our phonetic dictionary";
            cout<< "Enter a different word now, or watch this program crash and burn: ";
            string temp;
            cin >> temp;
            
            nextWordSAMPAPhoneSeqs = getPhoneSeqsForOrthoWord( temp );
         }
         
         //if this is ths first orthoWord
         if( i == 0 ) {
            for( int j = 0; j < nextWordSAMPAPhoneSeqs.size(); j++ ) {
               fullPhrasePhoneSeqs.push_back( nextWordSAMPAPhoneSeqs[j] );
            }
         } else {
            int numFullPhrases = fullPhrasePhoneSeqs.size();
            //cerr << "\tnumFullPhrases = "<< numFullPhrases << endl;//TODO debug
            if ( nextWordSAMPAPhoneSeqs.size() > 1 ) {
               //cerr << "\tnextWordSAMPAPhoneSeqs.size() = "<< nextWordSAMPAPhoneSeqs.size() << endl;//TODO debug
               for(int m = 1; m < nextWordSAMPAPhoneSeqs.size(); m++) {
                  //if there's more than one phonetic interpretation of the 
                  // ortho word to be added, then we need to create duplicates 
                  // of all existing sampaPhrase entries for each of them.
                  for( int n = 0; n < numFullPhrases; n++){
                     vector< phone > copyOfFullPhraseN( fullPhrasePhoneSeqs[n] ); 
                     fullPhrasePhoneSeqs.push_back( copyOfFullPhraseN );
                  }
               }
            }
            for( int m = 0; m < fullPhrasePhoneSeqs.size(); m++){
               int phrsToAppendNdx = m / numFullPhrases;
               vector<phone> phraseToAppend( nextWordSAMPAPhoneSeqs[phrsToAppendNdx] );
               fullPhrasePhoneSeqs[m].insert( fullPhrasePhoneSeqs[m].end(),
                                       phraseToAppend.begin(), 
                                       phraseToAppend.end() );
            }
                  //DEBUG
                  for ( int e = 0; e < fullPhrasePhoneSeqs.size(); e++ ) {
                     cerr << e <<"***sampa phrase after append  ";
                     for ( int f = 0; f < fullPhrasePhoneSeqs[e].size(); f++ ) {
                        cerr<< "_" << fullPhrasePhoneSeqs[e][f] << "_";
                     }
                     cerr << endl; 
                  }
                  //END DEBUG
         }
         /*
         //DEBUG
         cerr << j <<"++SAMPA+PHRASES++  ";
         for ( int k = 0; k < fullPhrasePhoneSeqs[j].size(); k++ ) {
            cerr<< "-" << fullPhrasePhoneSeqs[j][k] << "-";
         }
         cerr << endl; 
         //END DEBUG
      */
      
	}	
        // assert(0);

/*
   vector<string> misheard;
   for (int i = 0; i < fullPhrasePhoneSeqs.size(); i++){
   	 //misheard.push_back( interpretPhrase( fullPhrasePhoneSeqs[i] ) ) 
	}
	
	for (int i = 0; i < misheard.size(); i++) {
      string s = misheard[i];
      DDDDDDDDDDDEBUG(s);
	}	


	return misheard;
*/

   

   return fullPhrasePhoneSeqs;
}


/*given an phrase of ortho words, gives all the sampa permutations
 that it could possibly be, where each inner vector represents all the
 possible phonetic interpretations for each phoneme. For example:
 Given: a nice
 Returns:   vector[0]: { e, @, A }
            vector[1]: { n }
            vector[2]: { aI, i }
            vector[3]: { s }
             */
 vector< set<phone> > findPhoneTreeForOrthoPhrase( string orthoPhrase ) {
	vector< set<phone> > phoneTree;
	cerr <<"findPhoneTreeForOrthoPhrase is broken! ("<<orthoPhrase<<")"<<endl;
	assert(0);
   //using set because it doesn't allow for duplicates
	vector<string> orthoWords = strTokOnWhitespace( orthoPhrase );
	vector< vector<phone> > oldFullPhrasePhoneSeqs = findAllPhoneSeqsForOrthoPhrase( orthoPhrase );
	vector< vector<phone> > fullPhrasePhoneSeqs;
	
   if( oldFullPhrasePhoneSeqs.size() > 0 )   {
      for( int i = 0; i < oldFullPhrasePhoneSeqs.size(); i++) {
         //strip all the phoneSeqs of emph values
         fullPhrasePhoneSeqs.push_back( getNoEmphsPhoneVect( oldFullPhrasePhoneSeqs.at(i) ) );
      }      

      for(int i = 0; i < fullPhrasePhoneSeqs[0].size(); i++) {
         
         set<phone> dummySet;
         
         for( int j = 0; j < fullPhrasePhoneSeqs.size(); j++) {
            //I assume that all phone seqs will be equal length. If not, assert.
            if( fullPhrasePhoneSeqs[j].size() != fullPhrasePhoneSeqs[0].size() ){
               cerr << "0-size="<< fullPhrasePhoneSeqs[0].size()<< endl;
               cerr << j<<"-size="<< fullPhrasePhoneSeqs[j].size()<< endl;
               cerr << j<<"="<< phoneVectToString( fullPhrasePhoneSeqs[j] )<< endl;

               assert(0); 
            }
            //put the i-th phone from each fullPhrasePhoneSeq into the i-th phoneTree set.
            int deleteme1 = dummySet.size();
            phone toAdd = fullPhrasePhoneSeqs[j][i];
            cerr << "dummySet.size()="<<deleteme1<<",  toAdd="<<toAdd<<endl;
            dummySet.insert( toAdd );
         }
         phoneTree.push_back( dummySet );
      }
   }
   
   
   

   //DEBUG
   for ( int j = 0; j < phoneTree.size(); j++) {
      cerr << j <<"++PHONE+TREE++  ";
      vector<phone> temp( phoneTree[j].begin(), phoneTree[j].end() );

      for ( int k = 0; k < temp.size(); k++ ) {

         cerr<< "-" << temp.at(k) << "-";
      }
      cerr << endl; 
   }
   //END DEBUG

   return phoneTree;
}

vector<string> discoverOronymsForPhrase( string origOrthoPhrase, bool includeDeadends ) {
   vector<string> orthoMisheardAsPhrases;
   vector<vector<phone> > allPhoneSeqsOfOrigPhrase = findAllPhoneSeqsForOrthoPhrase( origOrthoPhrase );
   
   int numUniquePhoneticInterpretations = allPhoneSeqsOfOrigPhrase.size();
   for(int i = 0; i < numUniquePhoneticInterpretations; i++) {
      vector<phone> curPhoneSeqWithEmph( allPhoneSeqsOfOrigPhrase.at(i) );
      string strOfCurPhoneSeq = phoneVectToString( curPhoneSeqWithEmph );
      
      cerr << "Phonetic interpretation "<<i<<" ("<< strOfCurPhoneSeq <<")"<<endl;
      
      //remove emphasis marking for easier lookups
      vector<phone> curPhoneSeq = getNoEmphsPhoneVect( curPhoneSeqWithEmph );

      //vector<string> altOrthoPhrases = interpretPhrase( curPhoneSeq );
      vector<string> altOrthoPhrases = findOrthoStrsForPhoneSeq( curPhoneSeq );
      
      //cerr << "exits findOrthoStrsForPhoneSeq"<<endl;
      for( int j = 0; j < altOrthoPhrases.size(); j++) {
         string altOrthoPhrase = altOrthoPhrases.at(j);
         cerr << i <<"~~>" << altOrthoPhrase << endl;
         
         //ensure it contains no deadEndDelims so we only add fully valid strings
         if ( ( includeDeadends == true && altOrthoPhrase != deadEndDelim1 
                 && altOrthoPhrase != deadEndDelim2 )
            || ( altOrthoPhrase.find( deadEndDelim1 ) == string::npos 
                 && altOrthoPhrase.find( deadEndDelim2 ) == string::npos ) ) {  
              
            orthoMisheardAsPhrases.push_back( altOrthoPhrase );
         }
      }
   }
   //deduplicate orthoMisheardAsPhrases by putting in a set and back again
   cerr << "DEDUPLICATION TIME!" <<endl;
   set<string> tempSetForDeduplication( orthoMisheardAsPhrases.begin(), orthoMisheardAsPhrases.end() );
   orthoMisheardAsPhrases.assign( tempSetForDeduplication.begin(), tempSetForDeduplication.end() );
   return orthoMisheardAsPhrases;
}


/*given an orthoPhrase, returns all possible orthoPhrases it could be misheard as*/
vector<string> discoverOronymsForPhrase( string origOrthoPhrase ) {
   return discoverOronymsForPhrase( origOrthoPhrase, false );
}

/*This function does the phoneme-tree-traversal thing for oronyms
   returns orthographic phrases (I *think* each string is a full phrase...)*/
vector<string> interpretPhrase( vector<phone> sampaPhraseOrig ) {
   vector<phone> sampaPhrase = getNoEmphsPhoneVect(sampaPhraseOrig);
	vector<string> misheardOrthoPhrases;
   assert(0);
   /*
   cerr << "INTERPRET PHRASE for " << phoneVectToString(sampaPhrase) << endl;
	if( sampaPhrase.size() == 0 ) {
		misheardOrthoPhrases.push_back("");
		cerr << "<<<<<<phraseSize == 0, so returning all of the phrases" <<endl;
		return misheardOrthoPhrases;
	}
	
   string sampaStr = "";
	vector <phone> usedPhones;
   cerr << "sampaPhrase.size() ="<<sampaPhrase.size()<<endl;
   for (int i = 0; i < sampaPhrase.size(); i++) {
      cerr << "i = "<<i<<"; sampaPhrase[i]= phone p = "<<sampaPhrase[i]<<endl;
      phone p = sampaPhrase[i];
      if( strcmp( "\"", p.c_str() ) == 0) {
         assert(0);
         continue; //TODO incorporate someday, but ignore emphases for now.
      } else if ( strcmp( "$", p.c_str() ) == 0) {
         assert(0);
         continue; //TODO incorporate someday, but ignore emphases for now.
      } else if ( strcmp( "%", p.c_str() ) == 0) {
         assert(0);
         continue; //TODO incorporate someday, but ignore emphases for now
      }
		sampaStr += p;      
		cerr<< "Sampastr = "<<sampaStr<<endl;
		usedPhones.push_back(p);
		vector<string> orthoMatches = queryDBwithSampaForOrthoStrs( sampaStr );
		cerr << "orthoMatches.size() =="<<orthoMatches.size()<<endl;
		//DEBUG
		for(int o = 0; o < orthoMatches.size(); o++) {
		   cerr<<"++"<<orthoMatches.at(o);
		}
		cerr<<endl;
		//END DEBUG		
		//if there are no exact matches
      if ( orthoMatches.size() == 0 ) {
		   vector<string> prefixMatches = queryDBForOrthoStrsWithSampaPrefix( sampaStr );
		   //if there are no partial matches, we have a dead end, so exit
		   if( prefixMatches.size() == 0 ) {
			   misheardOrthoPhrases.push_back( deadEndDelim1 );
			   //TODO might have to delete rest of phone seq? we'll see.
            continue;
			} else {
			   continue; //go to next loop iter and add next phone 
			}
			//return misheardOrthoPhrases;
			cerr <<" OLD RETURN STATEMENT WAS HERE for if no exact matches"<< endl;
		}

      for (int j = 0; j < orthoMatches.size(); j++) {
      cerr << "enter orthomatches loop"<<endl;
         string orthoWord = orthoMatches[j];
         cerr << "----"<<i<<"--orthoword--"<< orthoMatches[j] << endl;
         vector<phone> sampaPhraseTail( sampaPhrase.begin()+j+1, sampaPhrase.end() );
         cerr << "----"<<i<<"--sampaPhraseTail--"<< phoneVectToString(sampaPhraseTail) <<"|--"<< endl;

			vector<string> orthoLeaves = interpretPhrase ( sampaPhraseTail );
			if ( orthoLeaves.size() == 0 ) {
			   if( sampaPhraseTail.size() > 0 ) {
				   cerr<< "--"<<orthoWord<<"---no leaves, has tail: "<< phoneVectToString(sampaPhraseTail) <<endl;
				   //TODO RESTART TRACE AT NEXT LINE!perhaps want a continue?
				   misheardOrthoPhrases.push_back( orthoWord.append(  deadEndDelim1  ) );
				}
				//return misheardOrthoPhrases;
				cerr <<" OLD RETURN STATEMENT WAS HERE for if no ortholeaves"<< endl;
				continue;
			} else {
                        
            for (int k = 0; k < orthoLeaves.size(); k++) {
               string orthoLeaf = orthoLeaves[k];
					misheardOrthoPhrases.push_back( orthoWord + orthoLeaf );
				}
            
			}
		}
	}
	
	cerr<<"EXITING interpretPhrase"<<endl;
	return misheardOrthoPhrases;
	*/
}

vector<string> findOrthoStrsForPhoneSeq( vector<phone> phoneSeq ) {
	//cerr<<"+++findOrthoStrsForPhoneSeq, for "<< phoneVectToString( phoneSeq );
	//cerr<<",  size = "<<phoneSeq.size()<<endl;
	vector<phone> usedPhonesForOrtho;
	
	vector<phone> curPhoneSeq;
	
	vector< string > fullOrthoStrs;
	if( phoneSeq.size() == 0 ) {
	   //cerr<<"+++ PHONE SEQ SIZE = 0, we're SUCCEEEDED! WOOOHOOO!"<<endl;
	   fullOrthoStrs.push_back(successDelim);
	   return fullOrthoStrs;
	}
	for ( int i = 0; i < phoneSeq.size(); i++) {
	   phone p = phoneSeq.at(i);
	   curPhoneSeq.push_back(p);
	   string curPhoneSeqStr = phoneVectToString( curPhoneSeq );
	   
	   //cerr<<"+++"<<"+++p"<<i<<":"<<p<<"  full subseq = "<<curPhoneSeqStr<<"."<<endl;
	   
	   /////STEP 1: EXACT MATCHES
	   //Query for exact ortho matches of the curPhoneSeq
	   vector<string> orthoInterps = queryDBwithSampaForOrthoStrs(curPhoneSeqStr);
	   
	   //cerr<<"+++"<<"+++orthoInterps.size() = "<<orthoInterps.size()<<endl;
	   
	   //If there is one or more exact ortho match for the phoneSeq
	   if( orthoInterps.size() > 0 ) {
	   
	      //Since we're using the phones in curPhoneSeq for our ortho matches, 
	      //we don't want to re-look-up those phonemes. the line below
	      // removes all the phones we used in curPhoneSeq from the phoneSeq to
	      // make newPhoneSeqTail;	      
	      vector<phone> newPhoneSeqTail( phoneSeq.begin() + i + 1 , phoneSeq.end() ); 
	      //cerr<<"+++++++++newPhoneSeqTail is "<< phoneVectToString( newPhoneSeqTail )<<endl;

	      //findOrthoStrings for the tail phonemes
	      vector<string> tailOrthoStrs = findOrthoStrsForPhoneSeq(newPhoneSeqTail);

	      //for each orthoInterpretation of the curPhoneSeq, 
	      for(  int j = 0; j < orthoInterps.size(); j++ ) {
	         for ( int k = 0; k < tailOrthoStrs.size(); k++ ) {
	            string headPlusTailOrtho = orthoInterps.at(j) + " "+ tailOrthoStrs.at(k);
	            fullOrthoStrs.push_back( headPlusTailOrtho );
	            cerr<<"+++"<<"+++"<<"+++"<<headPlusTailOrtho<<endl;
	         }
	      } 
	   } else if ( i == phoneSeq.size() - 1 ) {
         //then there are no phonemes left after this one.
         //it would be stupid to check for partials if there's nothing to append
         //so we DEADBEEF THAT SHIT
         
         fullOrthoStrs.push_back( deadEndDelim1 );
         

	   } else {
	      //STEP 2: PARTIAL PREFIX MATCHES
	      
	      // query for ortho matches that have the current phoneSeq as a prefix
	      vector<string> orthoPartials = queryDBForOrthoStrsWithSampaPrefix(curPhoneSeqStr);
	      
	      //if there are partial matches
	      if( orthoPartials.size() > 0 ) {
	         continue;
	      } else {
	         // there are no partial matches even. 
	         // How should I denote thsi?
	         fullOrthoStrs.push_back( deadEndDelim2 );
	         break;
	      }
	   }  
	}
   return fullOrthoStrs;
}

vector<string> queryDBforStrings( char* sqlQuery, string queryCallback4thArg ) {
   char* zErr;
   confirmDatabaseInitialization();
   
   /*The following line calls the callback function, passing its 4th arg as the 
    first param of the callback function.  The sqlite3_exec function 
    queries the database, then for every result that it gets, it calls the 
    callback function.*/
   int rc = sqlite3_exec(db, sqlQuery, callback, (void*)queryCallback4thArg.c_str(), &zErr);
   if ( rc != SQLITE_OK ) {
      cerr << "error in queryDBforStrings for sqlite3_exec, rc = " << rc<<endl;
      if ( zErr != NULL ) {
         fprintf(stderr, "SQL error: %s\n", zErr);
         sqlite3_free(zErr);
      }
   }
   vector<string> retStrings;
   /*
   cerr << "Database results size = " << databaseResults.size() << endl;
   printDatabaseResultsRows(); //TODO  remove DEBUG
   */
   for( int i = 0; i < databaseResults.size(); i++) {
      retStrings.push_back( delSpaces( databaseResults[i][0] ) );
      //cerr << "\t#-" << retStrings[i] << "-#\n" ; //TODO DEBUG output
   }
   //cerr<< endl; //TODO DEBUG
   databaseResults.clear();
   /*
   cerr << "After clear, database results size = " << databaseResults.size() << endl;
   printDatabaseResultsRows(); //TODO  remove DEBUG
   */
   return retStrings;

}


/* given ortho, returns freq val */
int queryDBwithOrthoForFreq( string orthoWord ) {
   confirmDatabaseInitialization();
   char* sqlQuery = (char*) malloc( sizeof(char*) * MAX_DATABASE_QUERY_LEN );
   
   //fprintf(stderr, "\queryDBwithOrthoForFreq, orthoWord = %s\n", orthoWord.c_str());
   
   string lowercaseOrthoWord = toLowerCase( orthoWord );

   sprintf(sqlQuery, "select freq from phoneticDictTable where lower(ortho) = \"%s\"",lowercaseOrthoWord.c_str()); 
   
   //I'm honestly not sure what's going on here, entirely.
   //from what I gather, it returns a vector of strings that should have at most 
   // one string in it
   vector<string> SAMPAvals = queryDBforStrings( sqlQuery, lowercaseOrthoWord );
   int result = 0;


   //the string returned is a numerical value, and so we use stringstream
   // to turn it into an actual number. At least, that's what it looks like
   // this does, and it's been returning the correct values, so there's that!
   if( SAMPAvals.size() > 0 ) {
      stringstream( SAMPAvals.at(0) ) >> result;
   }
/*
   //The following line calls the callback function, passing its 4th arg as the 
    //first param of the callback function.  The sqlite3_exec function 
    //queries the database, then for every result that it gets, it calls the 
    //callback function.
   int rc = sqlite3_exec(db, sqlQuery, callback, (void*)orthoWord.c_str(), &zErr);
   if ( rc != SQLITE_OK ) {
      if ( zErr != NULL ) {
         fprintf(stderr, "SQL error: %s\n", zErr);
         sqlite3_free(zErr);
      }
   }
   //int SAMPAcolIndex = 3;
    
   vector<string> SAMPAvals;
   ///
   //cerr << "Database results size = " << databaseResults.size() << endl;
   //printDatabaseResultsRows(); //TODO  remove DEBUG
   //
   for( int i = 0; i < databaseResults.size(); i++) {
      SAMPAvals.push_back( delSpaces( databaseResults[i][0] ) );
      cerr << "!~" << SAMPAvals[i] << "!~" ; //TODO DEBUG output
   }
   cerr<< endl; //TODO DEBUG
   databaseResults.clear();
   //
   //cerr << "After clear, database results size = " << databaseResults.size() << endl;
   //printDatabaseResultsRows(); //TODO  remove DEBUG
   //
   */
   return result;
}


/* given ortho, returns SAMPAs */
vector<string> queryDBwithOrthoForSampaStrs( string orthoWord ) {
   confirmDatabaseInitialization();
   char* sqlQuery = (char*) malloc( sizeof(char*) * MAX_DATABASE_QUERY_LEN );
   
   fprintf(stderr, "\nqueryDBwithOrthoForSampaStrs, orthoWord = %s\n", orthoWord.c_str());
   
   string lowercaseOrthoWord = toLowerCase( orthoWord );

   sprintf(sqlQuery, "select SAMPA from phoneticDictTable where lower(ortho) = \"%s\"",lowercaseOrthoWord.c_str()); 
   
   vector<string> SAMPAvals = queryDBforStrings( sqlQuery, lowercaseOrthoWord );
/*
   //The following line calls the callback function, passing its 4th arg as the 
    //first param of the callback function.  The sqlite3_exec function 
    //queries the database, then for every result that it gets, it calls the 
    //callback function.
   int rc = sqlite3_exec(db, sqlQuery, callback, (void*)orthoWord.c_str(), &zErr);
   if ( rc != SQLITE_OK ) {
      if ( zErr != NULL ) {
         fprintf(stderr, "SQL error: %s\n", zErr);
         sqlite3_free(zErr);
      }
   }
   //int SAMPAcolIndex = 3;
    
   vector<string> SAMPAvals;
   ///
   //cerr << "Database results size = " << databaseResults.size() << endl;
   //printDatabaseResultsRows(); //TODO  remove DEBUG
   //
   for( int i = 0; i < databaseResults.size(); i++) {
      SAMPAvals.push_back( delSpaces( databaseResults[i][0] ) );
      cerr << "!~" << SAMPAvals[i] << "!~" ; //TODO DEBUG output
   }
   cerr<< endl; //TODO DEBUG
   databaseResults.clear();
   //
   //cerr << "After clear, database results size = " << databaseResults.size() << endl;
   //printDatabaseResultsRows(); //TODO  remove DEBUG
   //
   */
   return SAMPAvals;
}


/*looks up emphasis-free SAMPA str in phonetic dict, 
   returns a bunch of ortho strings whos SAMPA is prefixed with sampaPrefix*/
vector<string> queryDBForOrthoStrsWithSampaPrefix( string sampaPrefix ) {
   confirmDatabaseInitialization();
   char* sqlQuery = (char*) malloc( sizeof(char*) * MAX_DATABASE_QUERY_LEN );
   
   //fprintf(stderr, "\nqueryDBForOrthoStrsWithSampaPrefix, sampaPrefix = %s\n", sampaPrefix.c_str());
   
   string sampaStrNoEmph = stripSampaStrOfEmph( sampaPrefix );
   if(sampaStrNoEmph.size() > 0) {
      sampaStrNoEmph.append("%");
   }
   //cerr << "final queryterm: |"<<sampaStrNoEmph <<"|" << endl;
   sprintf(sqlQuery, "select ortho from phoneticDictTable where trim(SAMPAnoEmph) like \"%s\"",sampaStrNoEmph.c_str()); 
   
   vector<string> orthoMatches = queryDBforStrings( sqlQuery, sampaStrNoEmph );
   
   
   return orthoMatches;
}

/*looks up emphasis-free SAMPA str in phonetic dict, returns a bunch of ortho*/
vector<string> queryDBwithSampaForOrthoStrs( string sampaStr ) {
   confirmDatabaseInitialization();
   char* sqlQuery = (char*) malloc( sizeof(char*) * MAX_DATABASE_QUERY_LEN );
   
   fprintf(stderr, "~~~~~~~~~queryDBwithSAMPAForOrthoStrs, sampaStr = %s|\n", sampaStr.c_str());
   
   string sampaStrNoEmph = stripSampaStrOfEmph( sampaStr );

   sprintf(sqlQuery, "select ortho from phoneticDictTable where trim(SAMPAnoEmph) = trim(\"%s\")",sampaStrNoEmph.c_str()); 
   
   vector<string> orthoMatches = queryDBforStrings( sqlQuery, sampaStrNoEmph );
   
   return orthoMatches;
}


/* given ortho, returns entire row to databaseResults */
void queryDBwithOrthoForRow( string orthoWord ) {
   confirmDatabaseInitialization();
   char* sqlQuery = (char*) malloc( sizeof(char*) * MAX_DATABASE_QUERY_LEN );
   char* zErr;
   
   assert(0);//THIS function messes with the way that the other database query
             // functions count return vals! DON'T RUN IT. Here for debugging ONLY.
   fprintf(stderr, "queryDBwithOrthoForSampaStrs, orthoWord = %s\n", orthoWord.c_str());
   
   string lowercaseOrthoWord = toLowerCase( orthoWord );

   sprintf(sqlQuery, "select * from phoneticDictTable where lower(ortho) = \"%s\"",lowercaseOrthoWord.c_str()); 

   int rc = sqlite3_exec(db, sqlQuery, callback, (void*)orthoWord.c_str(), &zErr);
   if ( rc != SQLITE_OK ) {
      if ( zErr != NULL ) {
         fprintf(stderr, "SQL error: %s\n", zErr);
         sqlite3_free(zErr);
      }
   }
}  
 
vector<phone> parseSAMPAintoPhonemes( string sampaString ) {
   vector<phone> sampaSylls = splitSampaIntoLetters(sampaString);
   return sampaSylls;
}

vector<string> splitSampaIntoLetters(string phrase) {
   vector<string> tokens;
   string sampaCharacter;
   
   for (size_t i=0; i < phrase.length(); i++) {
      // We're not at the last character
      if (i+1 < phrase.length()) {
         // Get a two-character set.
       sampaCharacter = phrase.substr(i, 2);
       if ( sampaCharacter == "`r"
             || sampaCharacter == "3`"
             || sampaCharacter == "aI"
             || sampaCharacter == "aU"
             || sampaCharacter == "dZ"
             || sampaCharacter == "jU"
             || sampaCharacter == "ju"
             || sampaCharacter == "OI"
             || sampaCharacter == "oU"
             || sampaCharacter == "ou"
             || sampaCharacter == "tS" ) {
          // We're good.  Skip the second character for purpose of the loop.
          i++;
         } else {
            sampaCharacter = phrase.at(i);
         }
      } else {
         sampaCharacter = phrase.at(i);
      }
      tokens.push_back(sampaCharacter);
   }
   return tokens;
}

string FirstWord(const string& line) {
   string firstWordToken = line.substr(0, line.find(' ') );
   return trimWhitespace( firstWordToken );
}

vector<string> getAllFirstWords( vector<string> origPhrases ) {
   cerr<< "##### getAllFirstWords, origPhrases.size() = " << origPhrases.size() << " ######" << endl;
   vector<string> firstWords;
   for(int i = 0; i < origPhrases.size(); i++) {
      string tempFirstWord = FirstWord( origPhrases[i] );
      cerr << "#orig phrase "<<i<<": \""<< origPhrases[i] <<"\";" ;
      cerr << "firstWord = "<< tempFirstWord << endl;
      
      //
      if( firstWords.empty() || std::find(firstWords.begin(), firstWords.end(), tempFirstWord) == firstWords.end() ) {
         cerr << "##FOUND WORD "<<tempFirstWord <<" in getAllFirstWords!" << endl;
         firstWords.push_back(tempFirstWord);
      } 
   }
   return firstWords;
}

vector< string> getAllOrthoTailPhrasesOf(string prefix, vector<string> fullPhrases) {
   cerr<< "##### getAllOrthoTailPhrasesOf, prefix = " << prefix << " ######" << endl;
   set<string> tailPhrases;
   for (int j = 0; j < fullPhrases.size(); j++) {
      cerr << "root phrase: "<< fullPhrases[j] << endl;
      if( prefix == FirstWord( fullPhrases.at(j) ) ) {
         cerr<<"curFirstWord='"<<prefix<<"';"; 
         //remove firstWord from those phrases
         string tempFull = fullPhrases.at(j);
         
         // find(' ') returns the index of the first space it finds in tempFull
         // we add one to find's result so that the substring starts after the space
         string tempTail = tempFull.substr( tempFull.find(' ') +1 );
         cerr<< "tail = '"<<tempTail<<"'"<<endl;

         if( tempTail != prefix ) {
            //If there were no spaces in the original phrase, then the prefix 
            // will be identical to the tail. This is because our substr/find
            // combo above won't work if there are no spaces. It'll return
            // string::npos, and string::npos+1 is one past the end position. 
            // for some reason, this just makes it assign the whole string to 
            // the tail? We'll see.
            //To handle the no-spaces case, we only add the tail to the tailPhrases 
            // if tempTail != prefix 
         
            string trimmedTempTail = trimWhitespace( tempTail );
         
            //if trimmedTempTail is a non-empty string, add it
            if ( trimmedTempTail.length() >= 1 ) {
               tailPhrases.insert( trimmedTempTail );
            }
         } 
      }
   }
   //convert set to vector
   vector<string> tailsVect( tailPhrases.begin(), tailPhrases.end() );
   ///*DEBUG
   cerr << "for '"<< prefix <<"', we have "<<tailsVect.size()<<" tailPhrases: "<<endl;
   for(int tp = 0;tp < tailsVect.size(); tp++ ) {
      cerr << "~~~"<<tp<<": "<< tailsVect.at(tp) <<endl;
   }
   //*///END DEBUG
   return tailsVect;
}

//http://www.daniweb.com/software-development/cpp/threads/27905
vector<string> strTokOnWhitespace(string phrase) {
   string temp;
   stringstream ss(phrase);
   vector<string> tokens;
   
   //while (phrase >> temp) {
   while ( getline(ss, temp, ' ') ) {
      tokens.push_back( delSpaces(temp) );
   }
   return tokens;
}

string toLowerCase( string data) {
   std::transform(data.begin(), data.end(), data.begin(), ::tolower);
   return data;
}

//From http://stackoverflow.com/a/8868204
string delSpaces(string &str) {
   str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
   return str;
}

string trimWhitespace( string &str) {
   //cerr << "trimWhitespace='"<<str<<"'"<<endl;
   string retStr = "";
   string whitespaces (" \t\f\v\n\r");
   size_t startpos = str.find_first_not_of(whitespaces);
   
   
   if( string::npos != startpos) {
      retStr = str.substr( startpos );
   }
   //cerr << "trimWhitespace='"<<str<<"'"<<endl;
   //cerr << "======ltrim===='"<<retStr<<"'"<<endl;

   size_t endpos = retStr.find_last_not_of(whitespaces);

   if ( endpos != string::npos ) {
      retStr.erase( endpos + 1 );
   } else {
      retStr.clear();
   }
   
   //cerr << "======rtrim===='"<<retStr<<"'"<<endl;
   
   return retStr; 
}

void getMaxAndMinFreqForAllOrthoPhrases( vector< string > fullPhrases, int* max, int* min ) {
   cerr << "getMaxAndMinFreqForAllOrthoPhrases, fullPhrases.size()="<<fullPhrases.size() << endl;

   int freqMax = DICTIONARY_MIN_FREQ - 1;
   int freqMin = DICTIONARY_MAX_FREQ + 1; 
   for(int i = 0; i < fullPhrases.size(); i++) {
      cerr << "i = "<<i << endl;
      vector< string > phraseWords =  strTokOnWhitespace ( fullPhrases[i] );
      for( int j = 0; j < phraseWords.size(); j++) {
         cerr << "---j = "<<j<<", word = "<< phraseWords.at(j) << endl;

         int freqTemp =  queryDBwithOrthoForFreq( phraseWords.at(j) );
         if( freqTemp > freqMax ) {
            freqMax = freqTemp;
         } else if ( freqTemp < freqMin ) {
            freqMin = freqTemp;
         }
      }
   }
   *min = freqMin;
   *max = freqMax;
}


int getTotalFreqForPhrase( string orthoPhrase ) {
   
   int freqSum = 0;
   
   vector<string> phraseWords = strTokOnWhitespace( orthoPhrase );
   
   //cerr << "--numPhraseWords = "<< phraseWords.size() <<endl;
   
   for(  int i = 0; i < phraseWords.size(); i++ ) {
      //cerr << "--["<<i<<"]:"<< phraseWords.at(i) <<endl;
      freqSum += queryDBwithOrthoForFreq( phraseWords.at(i) );
      
   }
   
   //assert(0);

   return freqSum;
}

string getCSVofWordFreqsForPhrase (string orthoPhrase ) {
   string csvLine = orthoPhrase;
   string delim = " , ";

   csvLine.append( delim );
   
   vector<string> phraseWords = strTokOnWhitespace( orthoPhrase );
   
   for(  int i = 0; i < phraseWords.size(); i++ ) {
      csvLine.append( phraseWords.at(i) );
      csvLine.append( delim );
      stringstream os;
      os << queryDBwithOrthoForFreq( phraseWords.at(i) );
      string freq_str = os.str(); //retrieve as a string
      csvLine.append( freq_str );
      csvLine.append( delim );

   }
   return csvLine;
   
}

vector<string> getCSVofAllPhraseFreqs( vector<string> orthoPhrases ) {

   vector<string> csvStrings;
   string delim = " , ";
   for (int i = 0; i < orthoPhrases.size(); i++) {
      cerr<<"."<<i;
      string csvLine = trimWhitespace( orthoPhrases.at( i ) );
      //cerr<<csvLine <<endl;
      int totalFreq = getTotalFreqForPhrase( csvLine );
      //cerr<<totalFreq <<endl;

      csvLine.append( delim );
      stringstream os;
      os << totalFreq;
      string freq_str = os.str(); //retrieve as a string
      csvLine.append( freq_str );
      csvStrings.push_back( csvLine );
   }
   cerr<<endl;
   return csvStrings;
}

vector< string > getCSVofEachPhrasesWordsFreqs( vector< string > orthoPhrases ) {
   vector<string> csvStrings;   
   for (int i = 0; i < orthoPhrases.size(); i++) {
      csvStrings.push_back( getCSVofWordFreqsForPhrase ( orthoPhrases.at(i) ) );
   }
   return csvStrings;
}

string stripSampaStrOfEmph(string &str) {
   str.erase(std::remove(str.begin(), str.end(), '"'), str.end());
   str.erase(std::remove(str.begin(), str.end(), '%'), str.end());
   str.erase(std::remove(str.begin(), str.end(), '$'), str.end());
   return str;
}

vector< phone > getNoEmphsPhoneVect(vector< phone > phoneVectOrig ) {
   vector<phone> phoneVectNoEmphs;
   for(int i = 0; i < phoneVectOrig.size(); i++) {
      phone p = phoneVectOrig.at(i);
      if( (p.compare("\"") != 0) 
          && (p.compare("$") != 0)
          && (p.compare("%") != 0) ) {
         phoneVectNoEmphs.push_back( p );
      }
   }
   return phoneVectNoEmphs;
}

string getSampaStrWithoutGlottalStops ( string sampaStr ) {
   string noGlottalsStr;
   vector< phone > tempPhoneVect = parseSAMPAintoPhonemes( sampaStr );
   tempPhoneVect = getNoGlottalStopsPhoneVect( tempPhoneVect );
   noGlottalsStr = phoneVectToString( tempPhoneVect );
   //assert(0);
   return noGlottalsStr;
} 

vector< phone > getNoGlottalStopsPhoneVect(vector< phone > phoneVectOrig ) {
   vector< phone > noGlottalsPhoneVect;
   assert( phoneVectOrig.size() > 0);
   //TODO: this is a gross simplification. Glottal stops: http://en.wikipedia.org/wiki/Glottal_stop
   noGlottalsPhoneVect.push_back( phoneVectOrig.at(0) );
   for(int i = 1; i < phoneVectOrig.size(); i++) {
      phone cur = phoneVectOrig.at(i);
      phone prev = noGlottalsPhoneVect.back();
      if( ( cur.compare("t") == 0) && isVowel( prev ) ) {
         continue; //don't add this t--it's a glottal stop
      } else {         
         noGlottalsPhoneVect.push_back( cur );
      }
   }
   return noGlottalsPhoneVect;
}


string getSampaStrWithoutContiguousDuplicatePhones ( string sampaStr ) {
   string noContigDupesStr;
   vector< phone > tempPhoneVect = parseSAMPAintoPhonemes( sampaStr );
   tempPhoneVect = getNoContigDupesPhoneVect( tempPhoneVect );
   noContigDupesStr = phoneVectToString( tempPhoneVect );
   //assert(0);
   return noContigDupesStr;
} 

vector< phone > getNoContigDupesPhoneVect(vector< phone > phoneVectOrig ) {
   vector< phone > noContigDupesPhoneVect;
   assert( phoneVectOrig.size() > 0);
   noContigDupesPhoneVect.push_back( phoneVectOrig.at(0) );
   for(int i = 1; i < phoneVectOrig.size(); i++) {
      phone cur = phoneVectOrig.at(i);
      phone prev = noContigDupesPhoneVect.back();
      if( (cur.compare(prev) != 0) ) {
         noContigDupesPhoneVect.push_back( cur );
      }
   }
   return noContigDupesPhoneVect;
}


vector< string > getVoicingVariationsOfSampaStr ( string sampaStr ) {
   assert(0);
}
vector< vector< phone > > getVoicingVariationsOfPhoneVect ( vector< phone >  phoneVectOrig ) {
   assert(0);
}


bool isVowel( phone p ) {
   bool isVowel = false;
   if( p == "A" 
      || p == "I" 
      || p == "E"
      || p == "3`"
      || p == "`r"
      || p == "{"
      || p == "u"
      || p == "@"
      || p == "jU"
      || p == "ju"
      || p == "i"
      || p == "V"
      || p == "U"
      || p == "e"
      || p == "O"
      || p == "aI"
      || p == "OI"
      || p == "oU"
      || p == "ou"
      || p == "aU"
      || p == "=" ) {
         isVowel = true;
      }
   return isVowel;
}


bool isVoiced( phone p ) {
   bool isVoiced = false;
   if( p == "b" 
      || p == "d" 
      || p == "dZ"
      || p == "g"
      || p == "v" 
      || p == "D"
      || p == "z"
      || p == "Z" 
      || p == "m"
      || p == "n"
      || p == "N" 
      || p == "l"
      || p == "r"
      || p == "w"
      || p == "y" ) {
         isVoiced = true;
      }
      return isVoiced;
}

bool isVoiceless( phone p ) {
   bool isVoiceless = false;
   assert(0);
   if( p == "p" 
      || p == "t" 
      || p == "tS"
      || p == "k"
      || p == "f" 
      || p == "T"
      || p == "s"
      || p == "S" 
      || p == "h"
      || p == "W"
      || p == "x" ) {
         isVoiceless = true;
      }
   return isVoiceless;
}

/* if p is a voiced phone, return its voiceless compliment
      p is a voiceless phone, return its voiced compliment
      p is not either of these, return p */
phone getPhoneWithInverseVoicing( phone p ) {
   phone toReturn;
   if( p == "p" ) {
      toReturn = "b";
   } else if ( p == "b" ) {
      toReturn = "p";
   } else if ( p == "t" ) {
      toReturn = "d";
   } else if ( p == "d" ) {
      toReturn = "t";
   } else if ( p == "tS" ) {
      toReturn = "dZ";
   } else if ( p == "dZ" ) {
      toReturn = "tS";
   } else if ( p == "k" ) {
      toReturn = "g";
   } else if ( p == "g" ) {
      toReturn = "k";
   } else if ( p == "f" ) {
      toReturn = "v";
   } else if ( p == "v" ) {
      toReturn = "f";
   } else if ( p == "T" ) {
      toReturn = "D";
   } else if ( p == "D" ) {
      toReturn = "T";
   } else if ( p == "s" ) {
      toReturn = "z";
   } else if ( p == "z" ) {
      toReturn = "s";
   } else if ( p == "S" ) {
      toReturn = "Z";
   } else if ( p == "Z" ) {
      toReturn = "S";
   } else if ( p == "w" ) {
      toReturn = "W";
   } else {
      toReturn = p;
   }
   return toReturn;
}



string phoneVectToString( std::vector< phone > phoneVect ) {
   string toRet = "";
   for( int i = 0; i < phoneVect.size(); i++ ) {
      toRet.append( phoneVect.at(i) );
   }
   return toRet;
}

void printDatabaseResultsRows() {
   for(int row = 0; row < databaseResults.size(); row++) {
      cerr << "ROW "<<row <<": ";
      for(int col = 0; col < databaseResults[row].size(); col++) {
         cerr << databaseResults[row][col] <<" | ";
      }
      cerr << endl;
   }
}

void DDDDDDDDDDDEBUG(string s) {
	cerr << s << endl;	
}
  
void connectToPhoneticDictionaryDatabase(string databaseFilename) {
   int rc;
   
   char* databaseName = (char*) malloc( sizeof(char*) * MAX_DATABASE_FILE_PATH_LEN );
   
   if( databaseFilename.empty() ) {
      databaseName = "/Users/admin/Documents/Thesis/SQLiteDatabases/phoneticDict";
   } else {
      sprintf(databaseName, "%s", databaseFilename.c_str());
   }

   rc = sqlite3_open(databaseName, &db);
   if( rc ) {
      fprintf( stderr, "Can't open database %s: %s\n", databaseName, sqlite3_errmsg( db ) );
      sqlite3_close( db );
      exit ( -1 );
   } else {
      
      cerr << "DATABASE SUCCESSFULLY OPENED" << endl;
      //DDDDDDDDDDDEBUG("DATABASE SUCCESSFULLY OPENED");
   }
}

void cleanupDatabase() {
   confirmDatabaseInitialization();
   sqlite3_close(db);
}
/*
vector<string>  getPhrasePhonemes(string phrase) {
	vector<string> phrasePhones;;
	vector<string> words = split(/\s/); //split on any whitespace character

	for ( string s : words) {
		vector<string> sampleSylls = getSAMPAForPhrase(s);
		//append to end
		phrasePhones.insert( phrasePhones.end(), 
			sampaSylls.begin(), sampaSylls.end() );			
	}
	return phrasePhones;
}
string[] getSAMPAForPhrase(string orthoWord) {
	//lookup in dictionary
	split sampa phrase
	$src_SAMPA =~ s/\s$//; #remove trailing whitespace;
	$src_SAMPA =~ s#^\s*##; # remove leading whitespace.
	
	print OUTFILE "$src_SAMPA\t";
	print OUTFILE ":\t";
	print "$src_SAMPA\t";

	@array_src_SAMPA = split (/[\$|%|\"]/, $src_SAMPA);
	
	//return split sampa phrase
}


vector<PhoneWithIndex> embedPhoneOrder(vector<string> phones) {
	vector<PhoneWithIndex> toRet;
	int i = 0;
	for( string s : phones) {
		toRet.add( LocPhone(i++, s) );  
	}
	return toRet;
}
*/

