<template>
  <uci-form config="oui-bwm">
    <uci-section type="bwm">
      <uci-option-input type="input" label="TTL" name="ttl" placeholder="30" rules="uinteger" append="s"></uci-option-input>
      <uci-option-dlist :label="$t('Local network')" name="local_network" :description="$t('192.168.1.1/24 or a logical interface name')"></uci-option-dlist>
    </uci-section>
  </uci-form>
</template>