/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/debug>
#include <cc/MountTable>

using namespace cc;
using namespace cc::testing;

class ComplexParseTest: public TestCase
{
    void run() override
    {
        String text =
            "sysfs /sys sysfs rw,nosuid,nodev,noexec,relatime 0 0\n"
            "proc /proc proc rw,nosuid,nodev,noexec,relatime 0 0\n"
            "udev /dev devtmpfs rw,nosuid,relatime,size=8152032k,nr_inodes=2038008,mode=755 0 0\n"
            "devpts /dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0\n"
            "tmpfs /run tmpfs rw,nosuid,noexec,relatime,size=1634984k,mode=755 0 0\n"
            "/dev/sdb1 / ext4 rw,relatime,errors=remount-ro,data=ordered 0 0\n"
            "securityfs /sys/kernel/security securityfs rw,nosuid,nodev,noexec,relatime 0 0\n"
            "tmpfs /dev/shm tmpfs rw,nosuid,nodev 0 0\n"
            "tmpfs /run/lock tmpfs rw,nosuid,nodev,noexec,relatime,size=5120k 0 0\n"
            "tmpfs /sys/fs/cgroup tmpfs rw,mode=755 0 0\n"
            "cgroup /sys/fs/cgroup/systemd cgroup rw,nosuid,nodev,noexec,relatime,xattr,release_agent=/lib/systemd/systemd-cgroups-agent,name=systemd 0 0\n"
            "pstore /sys/fs/pstore pstore rw,nosuid,nodev,noexec,relatime 0 0\n"
            "cgroup /sys/fs/cgroup/cpuset cgroup rw,nosuid,nodev,noexec,relatime,cpuset,clone_children 0 0\n"
            "cgroup /sys/fs/cgroup/blkio cgroup rw,nosuid,nodev,noexec,relatime,blkio 0 0\n"
            "cgroup /sys/fs/cgroup/perf_event cgroup rw,nosuid,nodev,noexec,relatime,perf_event,release_agent=/run/cgmanager/agents/cgm-release-agent.perf_event 0 0\n"
            "cgroup /sys/fs/cgroup/net_cls,net_prio cgroup rw,nosuid,nodev,noexec,relatime,net_cls,net_prio 0 0\n"
            "cgroup /sys/fs/cgroup/devices cgroup rw,nosuid,nodev,noexec,relatime,devices 0 0\n"
            "cgroup /sys/fs/cgroup/memory cgroup rw,nosuid,nodev,noexec,relatime,memory 0 0\n"
            "cgroup /sys/fs/cgroup/hugetlb cgroup rw,nosuid,nodev,noexec,relatime,hugetlb,release_agent=/run/cgmanager/agents/cgm-release-agent.hugetlb 0 0\n"
            "cgroup /sys/fs/cgroup/cpu,cpuacct cgroup rw,nosuid,nodev,noexec,relatime,cpu,cpuacct 0 0\n"
            "cgroup /sys/fs/cgroup/pids cgroup rw,nosuid,nodev,noexec,relatime,pids,release_agent=/run/cgmanager/agents/cgm-release-agent.pids 0 0\n"
            "cgroup /sys/fs/cgroup/freezer cgroup rw,nosuid,nodev,noexec,relatime,freezer 0 0\n"
            "systemd-1 /proc/sys/fs/binfmt_misc autofs rw,relatime,fd=30,pgrp=1,timeout=0,minproto=5,maxproto=5,direct 0 0\n"
            "mqueue /dev/mqueue mqueue rw,relatime 0 0\n"
            "debugfs /sys/kernel/debug debugfs rw,relatime 0 0\n"
            "hugetlbfs /dev/hugepages hugetlbfs rw,relatime 0 0\n"
            "fusectl /sys/fs/fuse/connections fusectl rw,relatime 0 0\n"
            "proc /srv/debby-nine-clean/proc proc rw,relatime 0 0\n"
            "sysfs /srv/debby-nine-clean/sys sysfs rw,relatime 0 0\n"
            "proc /srv/debby-nine/proc proc rw,relatime 0 0\n"
            "/dev/loop0 /snap/core/4571 squashfs ro,nodev,relatime 0 0\n"
            "/dev/loop1 /snap/core/4486 squashfs ro,nodev,relatime 0 0\n"
            "sysfs /srv/debby-nine/sys sysfs rw,relatime 0 0\n"
            "/dev/loop2 /snap/core/4650 squashfs ro,nodev,relatime 0 0\n"
            "/dev/loop3 /snap/magic-device-tool/299 squashfs ro,nodev,relatime 0 0\n"
            "binfmt_misc /proc/sys/fs/binfmt_misc binfmt_misc rw,relatime 0 0\n"
            "cgmfs /run/cgmanager/fs tmpfs rw,relatime,size=100k,mode=755 0 0\n"
            "lxcfs /var/lib/lxcfs fuse.lxcfs rw,nosuid,nodev,relatime,user_id=0,group_id=0,allow_other 0 0\n"
            "tmpfs /run/user/1001 tmpfs rw,nosuid,nodev,relatime,size=1634984k,mode=700,uid=1001,gid=1001 0 0\n"
            "/home/fme/.Private /home/fme ecryptfs rw,nosuid,nodev,relatime,ecryptfs_fnek_sig=e944ab8207926335,ecryptfs_sig=6d0849c33cc9c1a6,ecryptfs_cipher=aes,ecryptfs_key_bytes=16,ecryptfs_unlink_sigs 0 0\n"
            "/dev/sdb1 /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3 ext4 rw,relatime,errors=remount-ro,data=ordered 0 0\n"
            "proc /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/proc proc rw,nosuid,nodev,noexec,relatime 0 0\n"
            "sysfs /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/sys sysfs rw,nosuid,nodev,noexec,relatime 0 0\n"
            "udev /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/dev devtmpfs rw,nosuid,relatime,size=8152032k,nr_inodes=2038008,mode=755 0 0\n"
            "devpts /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/dev/pts devpts rw,nosuid,noexec,relatime,gid=5,mode=620,ptmxmode=000 0 0\n"
            "/dev/sdb1 /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/home ext4 rw,relatime,errors=remount-ro,data=ordered 0 0\n"
            "/home/fme/.Private /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/home/fme ecryptfs rw,nosuid,nodev,relatime,ecryptfs_fnek_sig=e944ab8207926335,ecryptfs_sig=6d0849c33cc9c1a6,ecryptfs_cipher=aes,ecryptfs_key_bytes=16,ecryptfs_unlink_sigs 0 0\n"
            "/dev/sdb1 /var/lib/schroot/mount/debby-nine-9974f1eb-2874-463d-af58-7d0aad86acb3/tmp ext4 rw,relatime,errors=remount-ro,data=ordered 0 0\n"
            "gvfsd-fuse /run/user/1001/gvfs fuse.gvfsd-fuse rw,nosuid,nodev,relatime,user_id=1001,group_id=1001 0 0\n"
            "# /dev/sdc1 /media/fme/3D85-AC8E vfat rw,nosuid,nodev,relatime,uid=1001,gid=1001,fmask=0022,dmask=0022,codepage=437,iocharset=iso8859-1,shortname=mixed,showexec,utf8,flush,errors=remount-ro 0 0\n";

        auto table = MountTable::parse(text);
        for (auto entry: table)
            fout() << entry->source() << " -> " << entry->target() << " (" << entry->type() << ": " << entry->options() << ")" << nl;

        CC_VERIFY(table->count() == 49);
        CC_VERIFY(table->at(table->count() - 1)->source() == "gvfsd-fuse");
        CC_VERIFY(table->at(table->count() - 1)->target() == "/run/user/1001/gvfs");
        CC_VERIFY(table->at(table->count() - 1)->type() == "fuse.gvfsd-fuse");
        CC_VERIFY(table->at(table->count() - 1)->options() == "rw,nosuid,nodev,relatime,user_id=1001,group_id=1001");
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ComplexParseTest);

    return TestSuite::instance()->run(argc, argv);
}
