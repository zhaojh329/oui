<template>
  <UciMap config="system">
    <UciSection name="system" title="System Properties" typed>
      <Tabs value="general">
        <TabPane label="General Settings" name="general">
          <UciInputValue name="hostname" title="Hostname"></UciInputValue>
        </TabPane>
        <TabPane label="Logging" name="logging">
          <UciInputValue name="log_size" title="System log buffer size"></UciInputValue>
          <UciInputValue name="log_ip" title="External system log server"></UciInputValue>
          <UciInputValue name="log_port" title="External system log server port"></UciInputValue>
        </TabPane>
      </Tabs>
    </UciSection>
  </UciMap>
</template>

<script>

export default {
  name: 'system'
}
</script>
