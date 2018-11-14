enum SymptomIDs {

	SYMPTOM_COUGH = 1,
	SYMPTOM_VOMIT,
	SYMPTOM_BLINDNESS,
	SYMPTOM_BULLET_HIT,
	SYMPTOM_BLEEDING_SOURCE,
	SYMPTOM_BLOODLOSS,
	SYMPTOM_SNEEZE,
	SYMPTOM_FEVERBLUR,
	SYMPTOM_LAUGHTER,
	SYMPTOM_UNCONSCIOUS,
};

enum SymptomTypes 
{
	PRIMARY,
	SECONDARY
};

const int DEBUG_PADDING_OFFSET = 2;
const int MAX_QUEUE_SIZE = 5; 

class SymptomManager
{
	PlayerBase m_Player;
	ref map<int, ref SymptomBase> m_AvailableSymptoms;
	
	ref array<ref SymptomBase> m_SymptomQueuePrimary;
	ref array<ref SymptomBase> m_SymptomQueueSecondary;

	ref map<int , SymptomBase > m_SymptomsUIDs;

	
	ref array<ref Param> m_SymptomQueueServerDbg;
	ref array<ref Param> m_SymptomQueueServerDbgPrimary;
	ref array<ref Param> m_SymptomQueueServerDbgSecondary;
	//ref array<string> m_SymptomQueueSecondaryServerDbg;

	int m_AnimID;
	int m_SymptomUID;
	int m_StanceMask;
	float m_Duration;
	bool m_IsAnimSet;
	int m_ActiveSymptomIndexPrimary = -1;
	int m_CurrentCommandID;
	
	ref Timer m_Timer;
	
	bool m_ShowDebug = false;
	bool m_ShowDebug2 = false;
	
	void Init()
	{
		RegisterSymptom(new CoughSymptom);
		RegisterSymptom(new VomitSymptom);
		RegisterSymptom(new BlindnessSymptom);
		RegisterSymptom(new BulletHitSymptom);
		RegisterSymptom(new SneezeSymptom);
		RegisterSymptom(new FeverBlurSymptom);
		RegisterSymptom(new BloodLoss);
		RegisterSymptom(new LaughterSymptom);
		
	}

	void AutoactivateSymptoms()
	{
		if( GetGame().IsClient() ) 
		{
			return;
		}
		
		QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_BLOODLOSS);
	}
	
	void SymptomManager(PlayerBase player)
	{
		m_Timer = new Timer;
		m_SymptomsUIDs = new map<int , SymptomBase >;
		m_SymptomQueuePrimary = new array<ref SymptomBase>;
		m_SymptomQueueSecondary = new array<ref SymptomBase>;
		m_SymptomQueueServerDbg = new array<ref Param>;
		m_SymptomQueueServerDbgPrimary = new array<ref Param>;
		m_SymptomQueueServerDbgSecondary = new array<ref Param>;
		//m_SymptomQueueSecondaryServerDbg = new array<ref Param>;
		m_AvailableSymptoms = new map<int, ref SymptomBase>;
		m_Player = player;
		Init();
		AutoactivateSymptoms();
	}
	
	void OnPlayerKilled()
	{
		for(int i = 0; i < m_SymptomQueuePrimary.Count(); i++)
		{
			m_SymptomQueuePrimary.Get(i).OnOwnerKilled();
		}
		
		for(i = 0; i < m_SymptomQueueSecondary.Count(); i++)
		{
			if(m_SymptomQueueSecondary.Get(i)) m_SymptomQueueSecondary.Get(i).OnOwnerKilled();
		}
	}

	SymptomBase GetSymptomByUID(int SYMPTOM_uid)
	{
		return m_SymptomsUIDs.Get(SYMPTOM_uid);
	}
	
	
	void ~SymptomManager()
	{
	}

	PlayerBase GetPlayer()
	{
		return m_Player;
	}
	
	//! Removes all  Symptoms of a given type
	/*
	void RemoveSecondarySymptomsByType( int type )
	{
		for(int i = 0; i < m_SymptomQueueSecondary.Count(); i++)
		{
			int current_type = m_SymptomQueueSecondary.Get(i).GetType();
			
			if( current_type == type )
			{
				m_SymptomQueueSecondary.Get(i).Destroy();
				i--;
			}
		}
	}
	*/
	
	void RegisterSymptom(SymptomBase Symptom)
	{
		Symptom.Init(this, m_Player,0);
		int id = Symptom.GetType();
		
		if( m_AvailableSymptoms.Contains(id) )
		{
			Error("SymptomBase Symptom already registered !");
			return;
		}
		
		m_AvailableSymptoms.Insert(id, Symptom);
		//PrintString("inserting id: "+ToString(id));
		
	
	}

	void OnAnimationFinished(int SYMPTOM_uid)
	{
		SymptomBase Symptom = GetSymptomByUID(SYMPTOM_uid);
		if( Symptom ) Symptom.OnAnimationFinish();
	}
	
	int CreateUniqueID()
	{
		int uid = Math.RandomInt( 1, 2147483647);
		if( !IsUIDUsed(uid) ) return uid;
		else return CreateUniqueID();
	}

	bool IsUIDUsed(int uid)
	{
		if( m_SymptomsUIDs.Contains(uid) ) 
		{
			return true;
		}
		else 
		{
			return false;
		}
	}
	
	string GetSymptomName(int symptom_id)
	{
		return m_AvailableSymptoms.Get(symptom_id).GetName();
	}
		
	//! Exits a specific Symptom with a given UID
	void RequestSymptomExit(int SYMPTOM_uid)
	{
		if( m_SymptomsUIDs.Get(SYMPTOM_uid) ) m_SymptomsUIDs.Get(SYMPTOM_uid).RequestDestroy();
	}

	bool IsSymptomPrimary(int symptom_id)
	{
		return m_AvailableSymptoms.Get(symptom_id).IsPrimary();
	}

	void OnInputUserDataReceived(ParamsReadContext ctx)
	{	
	}

	int GetCurrentCommandID()
	{
		return m_CurrentCommandID;
	}
	
	void OnScheduledTick(float deltatime, int pCurrentCommandID)
	{
		m_CurrentCommandID = pCurrentCommandID;
		if(m_ActiveSymptomIndexPrimary == -1)
		{
			m_ActiveSymptomIndexPrimary = FindFirstAvailableSymptomIndex();
		}
		
		UpdateActiveSymptoms(deltatime);
		if(m_IsAnimSet) 
		{
			PlayAnimation();
		}
		#ifdef DEVELOPER
		if( GetGame().IsMultiplayer() && GetGame().IsServer() ) return;//must be here !!!
		if ( DiagMenu.GetBool(DiagMenuIDs.DM_PLAYER_SYMPTOMS_SHOW) )
		{
			//DisplayDebug(true);
			array<ref Param>  primary_debug = PrepareClientDebug(m_SymptomQueuePrimary);
			array<ref Param>  secondary_debug = PrepareClientDebug(m_SymptomQueueSecondary);

			DisplayDebug1("Symptoms Client", 50,primary_debug, secondary_debug);
			DisplayDebug2("Symptoms Server", 300,m_SymptomQueueServerDbgPrimary, m_SymptomQueueServerDbgSecondary);
			ShowAvailableSymptoms();
		}
		else
		{
			CleanDebug1("Symptoms Client",50);
			CleanDebug2("Symptoms Server",300);
			CleanAvailableSymptoms();
		}
		#endif
	}

	void SetAnimation(ParamsReadContext ctx)
	{
		m_IsAnimSet = true;
		DayZPlayerSyncJunctures.ReadPlayerSymptomAnimParams(ctx, m_AnimID, m_SymptomUID, m_StanceMask, m_Duration);
	}
	
	
	void PlayAnimation()
	{
		HumanCommandActionCallback callback = GetPlayer().GetCommand_Action();
		if (!callback)
		{
			callback = GetPlayer().GetCommandModifier_Action();
		}
		if( callback )
		{
			callback.InternalCommand(DayZPlayerConstants.CMD_ACTIONINT_INTERRUPT);
		}
		
	
		SymptomCB	anim_callback = SymptomCB.Cast(GetPlayer().StartCommand_Action(m_AnimID, SymptomCB, m_StanceMask));
		anim_callback.Init(m_SymptomUID, m_Duration, GetPlayer());
		m_IsAnimSet = false;

	}
	
	
	
	
	void UpdateActiveSymptoms(float deltatime)
	{	
		//if( GetGame().IsClient() && !m_Player.IsPlayer() ) return;
		//primary
		if( GetCurrentPrimaryActiveSymptom() )
		{ 
			if( !GetCurrentPrimaryActiveSymptom().IsActivated() )
			{
				if ( GetCurrentPrimaryActiveSymptom().CanActivate() ) 
					GetCurrentPrimaryActiveSymptom().Activate();
			}
			if( GetCurrentPrimaryActiveSymptom().IsActivated() )
			{
				GetCurrentPrimaryActiveSymptom().Update(deltatime);
			}
		}
		//secondary
		for(int i = 0; i < m_SymptomQueueSecondary.Count(); i++)
		{
			if( m_SymptomQueueSecondary.Get(i) && !m_SymptomQueueSecondary.Get(i).IsActivated() )
			{
				m_SymptomQueueSecondary.Get(i).Activate();
			}
			else
			{
				if(m_SymptomQueueSecondary.Get(i)) m_SymptomQueueSecondary.Get(i).Update(deltatime);
			}
		}
	}
	
	
	void OnSymptomExit(SymptomBase Symptom, int uid)
	{
		bool is_primary;
		if( Symptom ) is_primary = Symptom.IsPrimary();
		
		if( m_SymptomsUIDs.Contains(uid) )
		{
			m_SymptomsUIDs.Remove(uid);		
		}
		else
		{
			Debug.LogError("Symptom with this UID does not exist", "PlayerSymptoms");
		}
		if(is_primary)
		{
			for(int i = 0; i < m_SymptomQueuePrimary.Count(); i++)
			{
				if( m_SymptomQueuePrimary.Get(i) == Symptom )
				{
					m_SymptomQueuePrimary.RemoveOrdered(i);
					break;
				}
			}
		}
		else
		{
			for(i = 0; i < m_SymptomQueueSecondary.Count(); i++)
			{
				if( m_SymptomQueueSecondary.Get(i) == Symptom )
				{
					m_SymptomQueueSecondary.RemoveOrdered(i);
					break;
				}
			}
		}
		m_ActiveSymptomIndexPrimary = -1;
		if( GetGame() ) SendServerDebugToClient();
	}

	SymptomBase SpawnSymptom( int symptom_id, int uid = -1 )
	{
		if( m_AvailableSymptoms.Get(symptom_id) )
		{
			SymptomBase Symptom = SymptomBase.Cast(m_AvailableSymptoms.Get(symptom_id).ClassName().ToType().Spawn());
			if(uid == -1)
			{
				uid = CreateUniqueID();
			}
			Symptom.Init(this, m_Player,uid);
			if( m_SymptomsUIDs.Contains(uid) ) Error("Symptoms: Unique ID already exists !");
			m_SymptomsUIDs.Insert(uid, Symptom);
			return Symptom;
		}
		
		if( !Symptom ) 
		{
			Error("Symptom not registered");
		}
		
		return NULL;
	}
	
	void CleanUpPrimaryQueue()
	{
		m_SymptomQueuePrimary.Clear();
	}

	SymptomBase QueueUpPrimarySymptom(int symptom_id, int uid = -1)
	{
		SymptomBase Symptom;
		for(int i = 0; i < m_SymptomQueuePrimary.Count(); i++)
		{
			if( ComparePriority( GetSymptomPriority(symptom_id), m_SymptomQueuePrimary.Get(i).GetPriority() ) == 1 )
			{
				Symptom = SpawnSymptom( symptom_id, uid );
				m_SymptomQueuePrimary.InsertAt(Symptom,i);

				if( m_SymptomQueuePrimary.Count() > MAX_QUEUE_SIZE )
				{
					delete m_SymptomQueuePrimary.Get(MAX_QUEUE_SIZE);//delete the last element in the queue, no need to remove from the array, that's done via Symptom callback on destruct
				}
				break;
			}
		}
		if( !Symptom && m_SymptomQueuePrimary.Count() < MAX_QUEUE_SIZE) 
		{
			Symptom = SpawnSymptom( symptom_id, uid );
			m_SymptomQueuePrimary.Insert(Symptom);
		}
		SendServerDebugToClient();
		return Symptom;
	}
	
	void QueueUpSecondarySymptom(int symptom_id, int uid = -1)
	{
		SymptomBase Symptom = SpawnSymptom( symptom_id, uid);
		
		if( Symptom.IsPrimary() ) 
		{
			Error("trying to queue up a primary Symptom into a secondary Symptom queue");
			delete Symptom;
			return;
		}
		m_SymptomQueueSecondary.Insert(Symptom);
	}
	
	//! Removes a single Symptom
	void RemoveSecondarySymptom(int symptom_id)
	{
		for(int i = 0; i < m_SymptomQueueSecondary.Count();i++)
		{
			if( m_SymptomQueueSecondary.Get(i) && m_SymptomQueueSecondary.Get(i).GetType() == symptom_id )
			{
				m_SymptomQueueSecondary.Get(i).RequestDestroy();
				return;
			}
		}
	}
	
/*
	SymptomBase GetCurrentPrimaryActiveSymptom()
	{
		if(m_SymptomQueuePrimary.Count() > 0 ) return m_SymptomQueuePrimary.Get(0);
		else return NULL;
	}	
	*/
	SymptomBase GetCurrentPrimaryActiveSymptom()
	{
		if( GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{ 
			if( m_ActiveSymptomIndexPrimary >= 0 && m_ActiveSymptomIndexPrimary < m_SymptomQueuePrimary.Count() )
			{
				if( m_SymptomQueuePrimary.Get(m_ActiveSymptomIndexPrimary) ) return m_SymptomQueuePrimary.Get(m_ActiveSymptomIndexPrimary);
			}
		}
		if( GetGame().IsClient() || !GetGame().IsMultiplayer() )
		{
			if( m_SymptomQueuePrimary.Count() > 0 ) 
				return m_SymptomQueuePrimary.Get(0);
		}
		return NULL;
	}
	
	int FindFirstAvailableSymptomIndex()
	{
		for(int i = 0; i < m_SymptomQueuePrimary.Count();i++)
		{
			if( m_SymptomQueuePrimary.Get(i).CanActivate() )
			{
				return i;
			}
		}
		return -1;
	}
	
	int ComparePriority( int prio1, int prio2 )
	{
		if( prio1 > prio2 )
		{
			return 1;
		}
		else if ( prio2 > prio1 )
		{
			return 2;
		}
		return 0;
	}
	
	int GetSymptomPriority(int symptom_id)
	{
		return m_AvailableSymptoms.Get(symptom_id).GetPriority();
	}
	
	void OnRPC(int rpc_type, ParamsReadContext ctx)
	{
		ctx.Read(CachedObjectsParams.PARAM2_INT_INT);
		int symptom_id = CachedObjectsParams.PARAM2_INT_INT.param1;
		int uid = CachedObjectsParams.PARAM2_INT_INT.param2;
		
		bool is_primary = m_AvailableSymptoms.Get(symptom_id).IsPrimary();
		
		if( rpc_type == ERPCs.RPC_PLAYER_SYMPTOM_ON )
		{
			if( is_primary )
			{
				CleanUpPrimaryQueue();
				QueueUpPrimarySymptom(symptom_id,uid);
			}
			else
			{
				QueueUpSecondarySymptom(symptom_id,uid);
			}
		}
		else if( rpc_type == ERPCs.RPC_PLAYER_SYMPTOM_OFF )
		{
			if( is_primary )
			{
				CleanUpPrimaryQueue();
			}
			else
			{
				RemoveSecondarySymptom( symptom_id );
			}
		}
	}

	void OnRPCDebug(int rpc_type, ParamsReadContext ctx)
	{
		if( !GetGame().IsDebug() ) return;
		if( rpc_type == ERPCs.RPC_PLAYER_SYMPTOMS_DEBUG )
		{
			int primary_Symptoms_count;
			int secondary_Symptoms_count;
			
			int symptom_id;
			

			if ( ctx.Read(CachedObjectsParams.PARAM1_INT) )
			{
				primary_Symptoms_count = CachedObjectsParams.PARAM1_INT.param1;
			}
			
			if ( ctx.Read(CachedObjectsParams.PARAM1_INT) )
			{
				secondary_Symptoms_count = CachedObjectsParams.PARAM1_INT.param1;
			}
			
			m_SymptomQueueServerDbg.Clear();
			m_SymptomQueueServerDbgPrimary.Clear();
			m_SymptomQueueServerDbgSecondary.Clear();

			int overall_count = primary_Symptoms_count + secondary_Symptoms_count;
			for(int i = 0; i < overall_count ; i++)
			{
				Param3<bool, int, int> p3 = new Param3<bool, int, int>(false,0,0);
				ctx.Read(p3);
				bool is_primary = p3.param1;
				
				
				if(is_primary)  
				{
					m_SymptomQueueServerDbgPrimary.Insert(p3);//   m_SymptomQueueServerDbg.Insert(p3);
				}
				else
				{
					m_SymptomQueueServerDbgSecondary.Insert(p3);
				}
				//PrintString("elements m_SymptomQueueServerDbgPrimary:" + m_SymptomQueueServerDbgPrimary.Count());
				//PrintString("elements m_SymptomQueueServerDbgSecondary:" + m_SymptomQueueServerDbgSecondary.Count());
				
			}
			/*
			for(i = 0; i < secondary_Symptoms_count; i++)
			{
				ctx.Read(CachedObjectsParams.PARAM1_STRING);
				m_SymptomQueueSecondaryServerDbg.Insert(CachedObjectsParams.PARAM1_STRING.param1);				
			}*/
			
		}
		else if( rpc_type == ERPCs.RPC_PLAYER_SYMPTOMS_EXIT )
		{
			ctx.Read(CachedObjectsParams.PARAM1_INT);
			int SYMPTOM_uid = CachedObjectsParams.PARAM1_INT.param1;
			RequestSymptomExit(SYMPTOM_uid);
		}

		else if( rpc_type == ERPCs.RPC_PLAYER_SYMPTOMS_DEBUG_ON)
		{
			ctx.Read(CachedObjectsParams.PARAM1_INT);
			int id = CachedObjectsParams.PARAM1_INT.param1;
			if( IsSymptomPrimary(id) )
			{
				QueueUpPrimarySymptom(id);
			}
			else
			{
				QueueUpSecondarySymptom(id);
			}
		}
	
	}
	
	void SendServerDebugToClient()
	{
		if( !GetGame().IsDebug() && !GetPlayer().IsPlayerSelected() ) return;
		array<ref Param> debug_list = new array<ref Param>;
		
		Param1<int> p1 = new Param1<int>(0);
		Param1<int> p2 = new Param1<int>(0);
		
		p1.param1 = m_SymptomQueuePrimary.Count();
		p2.param1 = m_SymptomQueueSecondary.Count();
		
		debug_list.Insert(p1);
		debug_list.Insert(p2);
		
		Param3<bool,int,int> p;
		

		bool is_primary;
		int symptom_id;
		int SYMPTOM_uid;

		for( int i = 0; i < m_SymptomQueuePrimary.Count(); i++ )
		{
			is_primary = m_SymptomQueuePrimary.Get(i).IsPrimary();
			symptom_id = m_SymptomQueuePrimary.Get(i).GetType();
			SYMPTOM_uid = m_SymptomQueuePrimary.Get(i).GetUID();
			
			p = new Param3<bool,int,int>(is_primary, symptom_id, SYMPTOM_uid );
			debug_list.Insert(p);
		}

		for( i = 0; i < m_SymptomQueueSecondary.Count(); i++ )
		{

			is_primary = m_SymptomQueueSecondary.Get(i).IsPrimary();
			symptom_id = m_SymptomQueueSecondary.Get(i).GetType();
			SYMPTOM_uid = m_SymptomQueueSecondary.Get(i).GetUID();
			
			p = new Param3<bool,int,int>(is_primary, symptom_id, SYMPTOM_uid );
			debug_list.Insert(p);
		}
		GetGame().RPC(GetPlayer(), ERPCs.RPC_PLAYER_SYMPTOMS_DEBUG, debug_list, true);
	}

	void OnStoreSave( ParamsWriteContext ctx )
	{
		ref array<int> 		m_SaveQueue = new array<int>;

		for( int i = 0; i < m_SymptomQueuePrimary.Count(); i++ )
		{
			if( m_SymptomQueuePrimary.Get(i).IsPersistent() )
			{
				m_SaveQueue.Insert( m_SymptomQueuePrimary.Get(i).GetType() );
			}
		}

		for( i = 0; i < m_SymptomQueueSecondary.Count(); i++ )
		{
			if( m_SymptomQueueSecondary.Get(i).IsPersistent() )
			{
				m_SaveQueue.Insert( m_SymptomQueueSecondary.Get(i).GetType() );
			}
		}

		ctx.Write( m_SaveQueue );

	}

	void OnStoreLoad( ParamsReadContext ctx )
	{
		ref array<int> m_SaveQueue;
		
		if(ctx.Read(m_SaveQueue))
		{
		
			for( int i = 0; i < m_SaveQueue.Count(); i++ )
			{
				int id = m_SaveQueue.Get(i);
	
				if( IsSymptomPrimary(id) )
				{
					QueueUpPrimarySymptom( id );
				}
				else
				{
					QueueUpSecondarySymptom( id );
				}
			}
		}
	}
	
	void DebugRequestExitSymptom(int SYMPTOM_uid)
	{
		Param1<int> p1 = new Param1<int>(SYMPTOM_uid);
		GetGame().RPCSingleParam(GetPlayer(), ERPCs.RPC_PLAYER_SYMPTOMS_EXIT, p1, true, GetPlayer().GetIdentity());
		//return;
		/*
		if( GetGame().IsClient() || ( GetGame().IsClient() && !GetGame().IsMultiplayer() )  )
		{
			Param1<int> p1 = new Param1<int>(SYMPTOM_uid);
			GetGame().RPCSingleParam(GetPlayer(), ERPCs.RPC_PLAYER_SymptomS_EXIT, p1, true, GetPlayer().GetIdentity());		
		}
		*/
	}

	
	array<ref Param> PrepareClientDebug(array<ref SymptomBase> Symptoms)
	{
		array<ref Param> debug_array = new array<ref Param>;
		
		Param3<bool, int, int> p3;
		
		for(int i = 0; i < Symptoms.Count(); i++)
		{
			bool is_primary		= Symptoms.Get(i).IsPrimary();
			int symptom_id	 	= Symptoms.Get(i).GetType();
			int SYMPTOM_uid 		= Symptoms.Get(i).GetUID();
			
			p3 = new Param3<bool,int,int>(is_primary, symptom_id, SYMPTOM_uid );
			debug_array.Insert(p3);
		}
		return debug_array;
	}



	void DisplayDebug1(string name, int y_offset, array<ref Param> Symptoms_primary, array<ref Param> Symptoms_secondary)
	{
		string primary;
		Param3<bool, int, int> p3 = new Param3<bool, int, int>(false,0,0);
		
		DbgUI.BeginCleanupScope();     
        DbgUI.Begin(name, 50, y_offset);
		DbgUI.Text("Primary: ");
		
		bool is_primary;
		int symptom_id;
		int SYMPTOM_uid;
		string SYMPTOM_name;
		
		for(int i = 0; i < Symptoms_primary.Count(); i++)
		{
			p3 = Param3<bool, int, int>.Cast(Symptoms_primary.Get(i));
			
			is_primary = p3.param1;
			symptom_id = p3.param2;
			SYMPTOM_uid = p3.param3;
			SYMPTOM_name = GetSymptomName(symptom_id);
			
			primary += SYMPTOM_name + " | ";
		}
		DbgUI.Text(primary);
		DbgUI.Text("Secondary: ");
		for(i = 0; i < Symptoms_secondary.Count(); i++)
		{
			p3 = Param3<bool, int, int>.Cast(Symptoms_secondary.Get(i));
			
			is_primary = p3.param1;
			symptom_id = p3.param2;
			SYMPTOM_uid = p3.param3;
			SYMPTOM_name = GetSymptomName(symptom_id);
			DbgUI.Text(SYMPTOM_name);
		}
		


        DbgUI.End();
        DbgUI.EndCleanupScope();		
	}
		

	void DisplayDebug2(string name, int y_offset, array<ref Param> Symptoms_primary, array<ref Param> Symptoms_secondary)
	{
		string primary;
		Param3<bool, int, int> p3 = new Param3<bool, int, int>(false,0,0);
		
		DbgUI.BeginCleanupScope();     
        DbgUI.Begin(name, 50, y_offset);
		DbgUI.Text("Primary: ");
		
		bool is_primary;
		int symptom_id;
		int SYMPTOM_uid;
		string SYMPTOM_name;
		
		for(int i = 0; i < Symptoms_primary.Count(); i++)
		{
			p3 = Param3<bool, int, int>.Cast(Symptoms_primary.Get(i));
			
			is_primary = p3.param1;
			symptom_id = p3.param2;
			SYMPTOM_uid = p3.param3;
			SYMPTOM_name = GetSymptomName(symptom_id);
			
			if(DbgUI.Button( i.ToString() + " " +SYMPTOM_name, 80))
			{
				PrintString("Symptom: "+ SYMPTOM_uid.ToString());
				DebugRequestExitSymptom(SYMPTOM_uid);
			}
		}

		DbgUI.Text("Secondary: ");
		for(i = 0; i < Symptoms_secondary.Count(); i++)
		{
			p3 = Param3<bool, int, int>.Cast(Symptoms_secondary.Get(i));
			
			is_primary = p3.param1;
			symptom_id = p3.param2;
			SYMPTOM_uid = p3.param3;
			SYMPTOM_name = GetSymptomName(symptom_id);
			
			if(DbgUI.Button( i.ToString() + " " +SYMPTOM_name, 80))
			{
				PrintString("Symptom: "+ SYMPTOM_uid.ToString());
				DebugRequestExitSymptom(SYMPTOM_uid);
			}
		}



        DbgUI.End();
        DbgUI.EndCleanupScope();		
	}

	void ShowAvailableSymptoms()
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin("available Symptoms", 300, 50);
		
		for(int i = 0; i < m_AvailableSymptoms.Count(); i++)
		{
			SymptomBase Symptom = m_AvailableSymptoms.GetElement(i);
			string SYMPTOM_name = Symptom.GetName();
			int symptom_id = Symptom.GetType();
			
			if(DbgUI.Button( i.ToString() + " " + SYMPTOM_name, 80))
			{
				DebugRequestActivateSymptom(symptom_id);
			}			
		}
		
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	

	void CleanAvailableSymptoms()
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin("available Symptoms", 300, 50);
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	
	void CleanDebug1(string name, int y_offset)
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin(name, 50, y_offset);
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}
	
	void CleanDebug2(string name, int y_offset)
	{
		DbgUI.BeginCleanupScope();
		DbgUI.Begin(name, 50, y_offset);
		DbgUI.End();
		DbgUI.EndCleanupScope();
	}

	void DebugRequestActivateSymptom(int symptom_id)
	{
		CachedObjectsParams.PARAM1_INT.param1 = symptom_id;
		if(GetPlayer()) GetGame().RPCSingleParam(GetPlayer(), ERPCs.RPC_PLAYER_SYMPTOMS_DEBUG_ON , CachedObjectsParams.PARAM1_INT, true, GetPlayer().GetIdentity());
	}

}