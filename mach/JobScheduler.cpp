#include <ftl/Guard.hpp>
#include <ftl/System.hpp>
#include <ftl/Semaphore.hpp>
#include <ftl/ProcessFactory.hpp>
#include <ftl/File.hpp>
#include "JobScheduler.hpp"

namespace mach
{

Ref<JobScheduler, Owner> JobScheduler::start(Ref<JobList> jobList, int concurrency)
{
	Ref<JobScheduler, Owner> newScheduler = new JobScheduler(jobList, concurrency);
	newScheduler->Thread::start();
	return newScheduler;
}

JobScheduler::JobScheduler(Ref<JobList> jobList, int concurrency)
	: jobList_(jobList),
	  slots_(Semaphore::create((concurrency > 0) ? concurrency : System::concurrency())),
	  running_(Semaphore::create()),
	  starting_(Mutex::create()),
	  factory_(ProcessFactory::create()),
	  runMap_(RunMap::create()),
	  status_(0),
	  totalCount_(jobList->length()),
	  finishCount_(0)
{
	factory_->setIoPolicy(Process::CloseInput);
}

JobScheduler::~JobScheduler()
{
	wait();
}

bool JobScheduler::collect(Ref<Job> *completedJob)
{
	if (finishCount_ == totalCount_) {
		*completedJob = 0;
		return false;
	}

	Ref<Job> job;

	while (true)
	{
		if (status_ == 0) {
			running_->acquire();
		}
		else {
			if (!running_->tryAcquire()) break;
		}

		int status = 0;
		pid_t pid = Process::wait(&status);
		{
			Guard<Mutex> guard(starting_);
			job = runMap_->value(pid);
		}

		Ref<File> file = job->process_->rawOutput();
		file->seek(0);
		job->outputText_ = file->readAll();
		job->status_ = status;

		{
			Guard<Instance> guard(this);
			if (status_ == 0) status_ = status;
		}
		++finishCount_;

		slots_->release();
		break;
	}

	*completedJob = job;
	return job;
}

void JobScheduler::run()
{
	for (int i = 0; i < jobList_->length(); ++i)
	{
		slots_->acquire();
		{
			Guard<Instance> guard(this);
			if (status_ != 0) break;
		}
		Ref<Job> job = jobList_->at(i);
		Ref<File, Owner> file = File::temp();
		factory_->setRawOutput(file);
		factory_->setRawError(file);
		{
			Guard<Mutex> guard(starting_);
			job->process_ = Process::start(job->command_, factory_);
			runMap_->insert(job->process_->id(), job);
		}
		running_->release();
	}
}

} // namespace mach
