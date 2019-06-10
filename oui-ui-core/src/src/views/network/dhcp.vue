<template>
  <uci-form config="dhcp" @apply="onApply">
    <uci-section title="Server Settings" type="dnsmasq">
      <uci-tab title="General Settings" name="general">
        <uci-option type="switch" label="Domain required" name="domainneeded" initial="1"></uci-option>
        <uci-option type="switch" label="Authoritative" name="authoritative" initial="1"></uci-option>
        <uci-option type="input" label="Local server" name="local" required></uci-option>
        <uci-option type="input" label="Local domain" name="domain" required></uci-option>
        <uci-option type="switch" label="Log queries" name="logqueries" initial="1"></uci-option>
        <uci-option type="dlist" label="DNS forwardings" name="server"></uci-option>
        <uci-option type="switch" label="Rebind protection" name="rebind_protection" initial="1"></uci-option>
        <uci-option type="switch" label="Allow localhost" name="rebind_localhost" depend="rebind_protection"></uci-option>
        <uci-option type="switch" label="Domain whitelist" name="rebind_domain" depend="rebind_protection"></uci-option>
      </uci-tab>
      <uci-tab title="Resolv and Hosts Files" name="files">
        <uci-option type="switch" label="Use /etc/ethers" name="readethers"></uci-option>
        <uci-option type="input" label="Leasefile" name="leasefile"></uci-option>
        <uci-option type="switch" label="Ignore resolve file" name="noresolv"></uci-option>
        <uci-option type="input" label="Resolve file" name="resolvfile" depend="noresolv == '0'"></uci-option>
        <uci-option type="switch" label="Ignore /etc/hosts" name="nohosts"></uci-option>
        <uci-option type="dlist" label="Additional Hosts files" name="addnhosts"></uci-option>
      </uci-tab>
      <uci-tab title="Advanced Settings" name="advanced">
        <uci-option type="switch" label="Suppress logging" name="quietdhcp"></uci-option>
        <uci-option type="switch" label="Allocate IP sequentially" name="sequential_ip"></uci-option>
        <uci-option type="switch" label="Filter private" name="boguspriv"></uci-option>
        <uci-option type="switch" label="Filter useless" name="filterwin2k"></uci-option>
        <uci-option type="switch" label="Localise queries" name="localise_queries"></uci-option>
        <uci-option type="switch" label="Expand hosts" name="expandhosts" initial="1"></uci-option>
        <uci-option type="switch" label="No negative cache" name="nonegcache"></uci-option>
        <uci-option type="switch" label="Strict order" name="strictorder"></uci-option>
        <uci-option type="dlist" label="Bogus NX Domain Override" name="bogusnxdomain"></uci-option>
        <uci-option type="input" label="DNS server port" name="port" placeholder="53"></uci-option>
        <uci-option type="input" label="DNS query port" name="queryport" placeholder="any"></uci-option>
        <uci-option type="input" label="Max. DHCP leases" name="dhcpleasemax" placeholder="unlimited"></uci-option>
        <uci-option type="input" label="Max. EDNS0 packet size" name="ednspacket_max" placeholder="1280"></uci-option>
        <uci-option type="input" label="Max. concurrent queries" name="dnsforwardmax" placeholder="150"></uci-option>
      </uci-tab>
    </uci-section>
    <uci-section title="DHCP Server" type="dhcp">
      <uci-tab title="General Settings" name="general">
        <uci-option type="dummy" label="Interface" name="interface"></uci-option>
        <uci-option type="switch" label="Ignore interface" name="ignore"></uci-option>
        <uci-option type="input" label="Start" name="start" placeholder="100"></uci-option>
        <uci-option type="input" label="Limit" name="limit" placeholder="150"></uci-option>
        <uci-option type="input" label="Leasetime" name="leasetime" placeholder="12h"></uci-option>
      </uci-tab>
      <uci-tab title="Advanced Settings" name="advanced">
        <uci-option type="switch" label="Dynamic DHCP" name="dynamicdhcp" initial="1"></uci-option>
        <uci-option type="switch" label="Force" name="force"></uci-option>
        <uci-option type="input" label="IPv4-Netmask" name="netmask"></uci-option>
      </uci-tab>
    </uci-section>
    <uci-section title="Static Leases" type="host" addable>
      <uci-option type="input" label="Hostname" name="name"></uci-option>
      <uci-option type="input" label="MAC-Address" name="mac" required></uci-option>
      <uci-option type="input" label="IPv4-Address" name="ip" required></uci-option>
    </uci-section>
  </uci-form>
</template>

<script>
export default {
  methods: {
    onApply() {
      this.$system.initRestart('dnsmasq');
    }
  }
}
</script>
