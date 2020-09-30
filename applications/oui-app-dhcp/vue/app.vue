<template>
  <oui-form uci-config="dhcp" @applied="onApplied">
    <oui-typed-section :title="$t('dhcp.Server Settings')" type="dnsmasq" :collapsible="false" v-slot="{ s }" style="margin-bottom: 20px">
      <a-tabs>
        <a-tab-pane key="general" :tab="$t('General Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Domain required')" name="domainneeded" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Authoritative')" name="authoritative" initial/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Local server')" name="local" required/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Local domain')" name="domain" required/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Log queries')" name="logqueries" initial/>
          <oui-form-item-list :uci-section="s" :label="$t('dhcp.DNS forwardings')" name="server"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Rebind protection')" name="rebind_protection" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Allow localhost')" name="rebind_localhost" depend="rebind_protection"/>
          <oui-form-item-list :uci-section="s" :label="$t('dhcp.Domain whitelist')" name="rebind_domain" depend="rebind_protection" rules="host"/>
        </a-tab-pane>
        <a-tab-pane key="resolv" :tab="$t('dhcp.Resolv and Hosts Files')">
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Use /etc/ethers')" name="readethers"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Leasefile')" name="leasefile"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Ignore resolve file')" name="noresolv"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Resolve file')" name="resolvfile" depend="!noresolv"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Ignore /etc/hosts')" name="nohosts"/>
          <oui-form-item-list :uci-section="s" :label="$t('dhcp.Additional Hosts files')" name="addnhosts"/>
        </a-tab-pane>
        <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Suppress logging')" name="quietdhcp"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Allocate IP sequentially')" name="sequential_ip"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Filter private')" name="boguspriv"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Filter useless')" name="filterwin2k"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Localise queries')" name="localise_queries"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Expand hosts')" name="expandhosts" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.No negative cache')" name="nonegcache"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Strict order')" name="strictorder"/>
          <oui-form-item-list :uci-section="s" :label="$t('dhcp.Bogus NX Domain Override')" name="bogusnxdomain"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.DNS server port')" name="port" placeholder="53" rules="port"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.DNS query port')" name="queryport" placeholder="any" rules="port"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Max DHCP leases')" name="dhcpleasemax" placeholder="unlimited" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Max EDNS0 packet size')" name="ednspacket_max" placeholder="1280" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Max concurrent queries')" name="dnsforwardmax" placeholder="150" rules="uinteger"/>
        </a-tab-pane>
      </a-tabs>
    </oui-typed-section>
    <oui-typed-section :title="$t('dhcp.DHCP Server')" type="dhcp"  v-slot="{ s }" style="margin-bottom: 20px">
      <a-tabs>
        <a-tab-pane key="general" :tab="$t('General Settings')">
          <oui-form-item-dummy :uci-section="s" :label="$t('dhcp.Interface')" name="interface"/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Ignore interface')" name="ignore"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Start')" name="start" placeholder="100" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Limit')" name="limit" placeholder="150" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp.Leasetime')" name="leasetime" placeholder="12h" :rules="validateLeasetime"/>
        </a-tab-pane>
        <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Dynamic DHCP')" name="dynamicdhcp" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp.Force')" name="force"/>
          <oui-form-item-input :uci-section="s" :label="$t('IPv4-Netmask')" name="netmask" rules="ip4addr"/>
        </a-tab-pane>
      </a-tabs>
    </oui-typed-section>
    <oui-typed-section :title="$t('dhcp.Static Leases')" type="host" :columns="hostColumns" addremove >
      <template #name="{ s }">
        <oui-form-item-input :uci-section="s" name="name" rules="hostname"/>
      </template>
      <template #mac="{ s }">
        <oui-form-item-select :uci-section="s" name="mac" required rules="macaddr" :options="arp.macaddrs" allow-create/>
      </template>
      <template #ip="{ s }">
        <oui-form-item-select :uci-section="s" name="ip" required rules="ip4addr" :options="arp.ipaddrs" allow-create/>
      </template>
    </oui-typed-section>
  </oui-form>
</template>

<script>
export default {
  data () {
    return {
      arp: {
        macaddrs: [],
        ipaddrs: []
      },
      hostColumns: [
        { name: 'name', label: this.$t('Hostname') },
        { name: 'mac', label: this.$t('MAC-Address'), width: 300 },
        { name: 'ip', label: this.$t('IPv4-Address'), width: 300 }
      ]
    }
  },
  methods: {
    validateLeasetime (v) {
      if (v === '') return

      if (v === '1m') return this.$t('dhcp.minimum is 2 minutes (2m).')

      if (/^\d+h$|m$/.test(v)) return

      return this.$t('Invalid format. Correct format: "12h" or "30m"')
    },
    getARPTable () {
      return this.$rpc.call('network', 'arp_table')
    },
    onApplied () {
      this.$system.initRestart('dnsmasq')
    }
  },
  created () {
    this.getARPTable().then(r => {
      const macaddrs = []
      const ipaddrs = []

      r.entries.forEach(arp => {
        if (arp.macaddr === '00:00:00:00:00:00') return

        macaddrs.push(arp.macaddr)
        ipaddrs.push(arp.ipaddr)

        this.arp.macaddrs = Array.from(new Set(macaddrs))
        this.arp.ipaddrs = Array.from(new Set(ipaddrs))
      })
    })
  }
}
</script>
