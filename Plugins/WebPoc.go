package Plugins

import (
	"github.com/aderfg/acning/Common"
	"github.com/aderfg/acning/WebScan"
)

// WebPoc 直接执行Web漏洞扫描
func WebPoc(info *Common.HostInfo) error {
	if Common.DisablePocScan {
		return nil
	}
	WebScan.WebScan(info)
	return nil
}
