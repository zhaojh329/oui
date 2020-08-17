<template>
  <oui-form uci-config="dhcp" @applied="onApplied">
    <oui-typed-section :title="$t('Server Settings')" type="dnsmasq" :collapsible="false" v-slot="{ s }" style="margin-bottom: 20px">
      <a-tabs>
        <a-tab-pane key="general" :tab="$t('General Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('Domain required')" name="domainneeded" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('Authoritative')" name="authoritative" initial/>
          <oui-form-item-input :uci-section="s" :label="$t('Local server')" name="local" required/>
          <oui-form-item-input :uci-section="s" :label="$t('Local domain')" name="domain" required/>
          <oui-form-item-switch :uci-section="s" :label="$t('Log queries')" name="logqueries" initial/>
          <oui-form-item-list :uci-section="s" :label="$t('DNS forwardings')" name="server"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Rebind protection')" name="rebind_protection" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('Allow localhost')" name="rebind_localhost" depend="rebind_protection"/>
          <oui-form-item-list :uci-section="s" :label="$t('Domain whitelist')" name="rebind_domain" depend="rebind_protection" rules="host"/>
        </a-tab-pane>
        <a-tab-pane key="resolv" :tab="$t('Resolv and Hosts Files')">
          <oui-form-item-switch :uci-section="s" :label="$t('Use /etc/ethers')" name="readethers"/>
          <oui-form-item-input :uci-section="s" :label="$t('Leasefile')" name="leasefile"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Ignore resolve file')" name="noresolv"/>
          <oui-form-item-input :uci-section="s" :label="$t('Resolve file')" name="resolvfile" depend="!noresolv"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Ignore /etc/hosts')" name="nohosts"/>
          <oui-form-item-list :uci-section="s" :label="$t('Additional Hosts files')" name="addnhosts"/>
        </a-tab-pane>
        <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('Suppress logging')" name="quietdhcp"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Allocate IP sequentially')" name="sequential_ip"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Filter private')" name="boguspriv"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Filter useless')" name="filterwin2k"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Localise queries')" name="localise_queries"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Expand hosts')" name="expandhosts" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('No negative cache')" name="nonegcache"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Strict order')" name="strictorder"/>
          <oui-form-item-list :uci-section="s" :label="$t('Bogus NX Domain Override')" name="bogusnxdomain"/>
          <oui-form-item-input :uci-section="s" :label="$t('DNS server port')" name="port" placeholder="53" rules="port"/>
          <oui-form-item-input :uci-section="s" :label="$t('DNS query port')" name="queryport" placeholder="any" rules="port"/>
          <oui-form-item-input :uci-section="s" :label="$t('Max. DHCP leases')" name="dhcpleasemax" placeholder="unlimited" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('Max. EDNS0 packet size')" name="ednspacket_max" placeholder="1280" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('Max. concurrent queries')" name="dnsforwardmax" placeholder="150" rules="uinteger"/>
        </a-tab-pane>
      </a-tabs>
    </oui-typed-section>
    <oui-typed-section :title="$t('DHCP Server')" type="dhcp"  v-slot="{ s }" style="margin-bottom: 20px">
      <a-tabs>
        <a-tab-pane key="general" :tab="$t('General Settings')">
          <oui-form-item-dummy :uci-section="s" :label="$t('Interface')" name="interface"/>
          <oui-form-item-switch :uci-section="s" :label="$t('Ignore interface')" name="ignore"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp-Start')" name="start" placeholder="100" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('dhcp-Limit')" name="limit" placeholder="150" rules="uinteger"/>
          <oui-form-item-input :uci-section="s" :label="$t('Leasetime')" name="leasetime" placeholder="12h" :rules="validateLeasetime"/>
        </a-tab-pane>
        <a-tab-pane key="advanced" :tab="$t('Advanced Settings')">
          <oui-form-item-switch :uci-section="s" :label="$t('Dynamic DHCP')" name="dynamicdhcp" initial/>
          <oui-form-item-switch :uci-section="s" :label="$t('dhcp-Force')" name="force"/>
          <oui-form-item-input :uci-section="s" :label="$t('IPv4-Netmask')" name="netmask" rules="ip4addr"/>
        </a-tab-pane>
      </a-tabs>
    </oui-typed-section>
    <oui-typed-section :title="$t('Static Leases')" type="host" :columns="hostColumns" addremove >
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

      if (v === '1m') return this.$t('minimum is 2 minutes (2m).')

      if (/^\d+h$|m$/.test(v)) return

      return this.$t('Invalid format. Correct format: "12h" or "30m"')
    },
    getARPTable () {
      return this.$ubus.call('oui.network', 'arp_table')
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
