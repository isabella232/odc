/*
 * (C) Copyright 1996-2012 ECMWF.
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 * In applying this licence, ECMWF does not waive the privileges and immunities 
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include <signal.h>
#include <fcntl.h>

#include "eclib/Mutex.h"
#include "eclib/PipeStream.h"

//-----------------------------------------------------------------------------

namespace eclib {

//-----------------------------------------------------------------------------

PipeStream::PipeStream():
	in_(-1),
	out_(-1)
{
	toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;

	SYSCALL(::pipe(toChild_));
	SYSCALL(::pipe(toParent_));

}

PipeStream::PipeStream(int in,int out):
	in_(in),
	out_(out)
{
	toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;
}

PipeStream::~PipeStream()
{
	::close(in_);
	::close(out_);
	::close(toChild_[0]);
	::close(toChild_[1]);
	::close(toParent_[0]);
	::close(toParent_[1]);
}

void PipeStream::parentProcess()
{
    /// @todo change this to sigaction
    
	::signal(SIGPIPE,SIG_IGN);

	in_  = toParent_[0];
	out_ = toChild_[1];

	::close(toChild_[0]);
	::close(toParent_[1]);

	// Avoid childs from other threads having opened file descriptors
	SYSCALL(fcntl(in_,F_SETFD,FD_CLOEXEC));
	SYSCALL(fcntl(out_,F_SETFD,FD_CLOEXEC));

	Log::debug() << "parentProcess : "<<
		in_ << '/' << out_ << " closing " << toChild_[0] << '/' << toParent_[1] << endl;

	toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;

}

void PipeStream::childProcess()
{
    /// @todo change this to sigaction

    ::signal(SIGPIPE,SIG_IGN);

	in_  = toChild_[0];
	out_ = toParent_[1];

	::close(toChild_[1]);
	::close(toParent_[0]);

	Log::debug() << "childProcess : "<<
		in_ << '/' << out_ << " closing " << toChild_[1] << '/' << toParent_[0] << endl;

	toChild_[0] = toChild_[1] = toParent_[0] = toParent_[1] = -1;

}

long PipeStream::write(const void *buf, long len)
{
	return ::write(out_,buf,len);
}

long PipeStream::read(void *buf, long len)
{
	return ::read(in_,buf,len);
}

//-----------------------------------------------------------------------------

} // namespace eclib
