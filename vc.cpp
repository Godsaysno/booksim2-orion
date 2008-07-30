/*vc.cpp
 *
 *this class describes a virtual channel in a router
 *it includes buffers and virtual channel state and controls
 *
 *This class calls the routing functions
 */

#include "booksim.hpp"
#include "vc.hpp"

VC::VC( const Configuration& config, int outputs ) :
  Module( )
{
  _Init( config, outputs );
}

VC::VC( const Configuration& config, int outputs, 
	Module *parent, const string& name ) :
  Module( parent, name )
{
  _Init( config, outputs );
}

VC::~VC( )
{
}

void VC::_Init( const Configuration& config, int outputs )
{
  _state      = idle;
  _state_time = 0;

  _size = int( config.GetInt( "vc_buf_size" ) );

  _route_set = new OutputSet( outputs );

  _occupied_cnt = 0;

  _total_cycles    = 0;
  _vc_alloc_cycles = 0;
  _active_cycles   = 0;
  _idle_cycles     = 0;

  _pri = 0;

  _out_port = 0 ;
  _out_vc = 0 ;

  _watched = false;
}

bool VC::AddFlit( Flit *f )
{
  bool success = false;

  if ( (int)_buffer.size( ) != _size ) {
    _buffer.push( f );
    success = true;
  } 

  return success;
}

Flit *VC::FrontFlit( )
{
  Flit *f;

  if ( !_buffer.empty( ) ) {
    f = _buffer.front( );
  } else {
    f = 0;
  }

  return f;
}

Flit *VC::RemoveFlit( )
{
  Flit *f;

  if ( !_buffer.empty( ) ) {
    f = _buffer.front( );
    _buffer.pop( );
  } else {
    f = 0;
  }

  return f;
}

bool VC::Empty( ) const
{
  return _buffer.empty( );
}

VC::eVCState VC::GetState( ) const
{
  return _state;
}

int VC::GetStateTime( ) const
{
  return _state_time;
}

void VC::SetState( eVCState s )
{
  _state = s;
  _state_time = 0;

  if ( s == active ) {
    Flit *f;
    
    f = FrontFlit( );
    if ( f ) {
      _pri = f->pri;
    }

    _occupied_cnt++;
  }
}

const OutputSet *VC::GetRouteSet( ) const
{
  return _route_set;
}

void VC::SetOutput( int port, int vc )
{
  _out_port = port;
  _out_vc   = vc;
}

int VC::GetOutputPort( ) const
{
  return _out_port;
}

int VC::GetOutputVC( ) const
{
  return _out_vc;
}

int VC::GetPriority( ) const
{
  return _pri;
}

int VC::GetSize() const
{
  return _buffer.size();
}

void VC::Route( tRoutingFunction rf, const Router* router, const Flit* f, int in_channel )
{  
  rf( router, f, in_channel, _route_set, false );
}

void VC::AdvanceTime( )
{
  _state_time++;

  _total_cycles++;
  switch( _state ) {
  case idle          : _idle_cycles++; break;
  case active        : _active_cycles++; break;
  case vc_spec_grant : _active_cycles++; break;
  case vc_alloc      : _vc_alloc_cycles++; break;
  case vc_spec       : _vc_alloc_cycles++; break;
  case routing       : break;
  }
}

// ==== Debug functions ====

void VC::SetWatch( bool watch )
{
  _watched = watch;
}

bool VC::IsWatched( ) const
{
  return _watched;
}

void VC::Display( ) const
{
//  cout << _fullname << " : "
//       << "idle " << 100.0 * (double)_idle_cycles / (double)_total_cycles << "% "
//       << "vc_alloc " << 100.0 * (double)_vc_alloc_cycles / (double)_total_cycles << "% "
//       << "active " << 100.0 * (double)_active_cycles / (double)_total_cycles << "% "
//       << endl;
  if ( _state != VC::idle ) {
    cout << _fullname << ":  state: "  ;
    switch (_state) {
    case routing: cout << "routing"; break ;
    case vc_alloc: cout << "vc_alloc"; break ;
    case active: cout << "active"; break ;
    case vc_spec: cout << "vc_spec"; break ;
    case vc_spec_grant: cout << "vc_sepc_grant"; break ;
    }
      
    cout << " out_port: " << _out_port
	 << " out_vc " << _out_vc 
	 << " fill: " << _buffer.size() 
	 << endl ;
  }
}