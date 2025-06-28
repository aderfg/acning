//go:build !windows

package Plugins

import "github.com/aderfg/acning/Common"

func MiniDump(info *Common.HostInfo) (err error) {
	return nil
}
