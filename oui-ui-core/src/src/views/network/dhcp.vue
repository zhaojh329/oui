<template>
  <uci-form config="dhcp" @applied="onApplied">
    <uci-section :title="$t('Server Settings')" type="dnsmasq">
      <uci-tab :title="$t('General Settings')" name="general">
        <uci-option-switch :label="$t('Domain required')" name="domainneeded" initial="1"></uci-option-switch>
        <uci-option-switch :label="$t('Authoritative')" name="authoritative" initial="1"></uci-option-switch>
        <uci-option-input :label="$t('Local server')" name="local" required></uci-option-input>
        <uci-option-input :label="$t('Local domain')" name="domain" required></uci-option-input>
        <uci-option-switch :label="$t('Log queries')" name="logqueries" initial="1"></uci-option-switch>
        <uci-option-dlist :label="$t('DNS forwardings')" name="server"></uci-option-dlist>
        <uci-option-switch :label="$t('Rebind protection')" name="rebind_protection" initial="1"></uci-option-switch>
        <uci-option-switch :label="$t('Allow localhost')" name="rebind_localhost" depend="rebind_protection"></uci-option-switch>
        <uci-option-dlist :label="$t('Domain whitelist')" name="rebind_domain" depend="rebind_protection" rules="host"></uci-option-dlist>
      </uci-tab>
      <uci-tab :title="$t('Resolv and Hosts Files')" name="files">
        <uci-option-switch :label="$t('Use /etc/ethers')" name="readethers"></uci-option-switch>
        <uci-option-input :label="$t('Leasefile')" name="leasefile"></uci-option-input>
        <uci-option-switch :label="$t('Ignore resolve file')" name="noresolv"></uci-option-switch>
        <uci-option-input :label="$t('Resolve file')" name="resolvfile" depend="!noresolv"></uci-option-input>
        <uci-option-switch :label="$t('Ignore /etc/hosts')" name="nohosts"></uci-option-switch>
        <uci-option-dlist :label="$t('Additional Hosts files')" name="addnhosts"></uci-option-dlist>
      </uci-tab>
      <uci-tab :title="$t('Advanced Settings')" name="advanced">
        <uci-option-switch :label="$t('Suppress logging')" name="quietdhcp"></uci-option-switch>
        <uci-option-switch :label="$t('Allocate IP sequentially')" name="sequential_ip"></uci-option-switch>
        <uci-option-switch :label="$t('Filter private')" name="boguspriv"></uci-option-switch>
        <uci-option-switch :label="$t('Filter useless')" name="filterwin2k"></uci-option-switch>
        <uci-option-switch :label="$t('Localise queries')" name="localise_queries"></uci-option-switch>
        <uci-option-switch :label="$t('Expand hosts')" name="expandhosts" initial="1"></uci-option-switch>
        <uci-option-switch :label="$t('No negative cache')" name="nonegcache"></uci-option-switch>
        <uci-option-switch :label="$t('Strict order')" name="strictorder"></uci-option-switch>
        <uci-option-dlist :label="$t('Bogus NX Domain Override')" name="bogusnxdomain"></uci-option-dlist>
        <uci-option-input :label="$t('DNS server port')" name="port" placeholder="53" rules="port"></uci-option-input>
        <uci-option-input :label="$t('DNS query port')" name="queryport" placeholder="any" rules="port"></uci-option-input>
        <uci-option-input :label="$t('Max. DHCP leases')" name="dhcpleasemax" placeholder="unlimited" rules="uinteger"></uci-option-input>
        <uci-option-input :label="$t('Max. EDNS0 packet size')" name="ednspacket_max" placeholder="1280" rules="uinteger"></uci-option-input>
        <uci-option-input :label="$t('Max. concurrent queries')" name="dnsforwardmax" placeholder="150" rules="uinteger"></uci-option-input>
      </uci-tab>
    </uci-section>
    <uci-section :title="$t('DHCP Server')" type="dhcp">
      <uci-tab :title="$t('General Settings')" name="general">
        <uci-option-dummy :label="$t('Interface')" name="interface"></uci-option-dummy>
        <uci-option-switch :label="$t('Ignore interface')" name="ignore"></uci-option-switch>
        <uci-option-input :label="$t('dhcp-Start')" name="start" placeholder="100" rules="uinteger"></uci-option-input>
        <uci-option-input :label="$t('dhcp-Limit')" name="limit" placeholder="150" rules="uinteger"></uci-option-input>
        <uci-option-input :label="$t('Leasetime')" name="leasetime" placeholder="12h" :rules="validateLeasetime"></uci-option-input>
      </uci-tab>
      <uci-tab :title="$t('Advanced Settings')" name="advanced">
        <uci-option-switch :label="$t('Dynamic DHCP')" name="dynamicdhcp" initial="1"></uci-option-switch>
        <uci-option-switch :label="$t('dhcp-Force')" name="force"></uci-option-switch>
        <uci-option-input :label="$t('IPv4-Netmask')" name="netmask" rules="ip4addr"></uci-option-input>
      </uci-tab>
    </uci-section>
    <uci-section :title="$t('Static Leases')" type="host" addable table>
      <uci-option-input :label="$t('Hostname')" name="name" rules="hostname"></uci-option-input>
      <uci-option-list :label="$t('MAC-Address')" name="mac" required rules="macaddr" :options="arp.macaddrs" allow-create></uci-option-list>
      <uci-option-list :label="$t('IPv4-Address')" name="ip" required rules="ip4addr" :options="arp.ipaddrs" allow-create></uci-option-list>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  data() {
    return {
      arp: {
        macaddrs: [],
        ipaddrs: []
      }
    }
  },
  methods: {
    validateLeasetime(v) {
      if (v === '')
        return;

      if (v === '1m')
        return this.$t('minimum is 2 minutes (2m).');

      if (/^\d+h$|m$/.test(v))
        return;

      return this.$t('Invalid format. Correct format: "12h" or "30m"');
    },
    getARPTable() {
      return this.$ubus.call('oui.network', 'arp_table');
    },
    onApplied() {
      this.$system.initRestart('dnsmasq');
    }
  },
  created() {
    this.getARPTable().then(r => {
      r.entries.forEach(arp => {
        if (arp.macaddr === '00:00:00:00:00:00')
          return;

        this.arp.macaddrs.push(arp.macaddr);
        this.arp.ipaddrs.push(arp.ipaddr);
      });
    });
  }
}
</script>
