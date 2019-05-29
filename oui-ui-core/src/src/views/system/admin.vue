<template>
  <div>
    <Tabs v-model="tab">
      <TabPane label="Router Password" name="password">
        <Card title="Router Password">
          <Form :label-width="200">
            <FormItem label="Password">
              <Input type="password" size="large" prefix="md-lock" v-model="password" />
            </FormItem>
            <FormItem label="Confirmation">
              <Input type="password" size="large" prefix="md-lock" v-model="confirmation" />
            </FormItem>
          </Form>
          <Button type="primary" style="margin-right: 10px" @click="setPassword">Save</Button>
        </Card>
      </TabPane>
      <TabPane label="SSH Access" name="dropbear">
        <UciMap config="dropbear">
          <UciSection name="dropbear" title="SSH Server" typed>
            <UciListValue name="Interface" title="Interface" :list="interfaces"></UciListValue>
            <UciInputValue name="Port" title="Port"></UciInputValue>
            <UciSwitchValue name="PasswordAuth" title="Password authentication" true-value="on" false-value="off" default-val="on"></UciSwitchValue>
            <UciSwitchValue name="RootPasswordAuth" title="Allow root logins with password" true-value="on" false-value="off" default-val="on"></UciSwitchValue>
            <UciSwitchValue name="GatewayPorts" title="Gateway ports" true-value="on" false-value="off" default-val="off"></UciSwitchValue>
          </UciSection>
        </UciMap>
      </TabPane>
      <TabPane label="SSH-Keys" name="sshkeys">
      </TabPane>
    </Tabs>

  </div>
</template>

<script>

export default {
  name: 'admin',
  data() {
    return {
      tab: 'password',
      password: '',
      confirmation: '',
      interfaces: []
    }
  },
  methods: {
    setPassword() {
      this.$session.get(r => {
        this.$system.setPassword(r.username, this.password).then(() => {
          this.$router.push('/login');
        });
      });
    }
  },
  mounted() {
    this.$network.load().then(() => {
      const interfaces = this.$network.getInterfaces();
      this.interfaces = interfaces.map(item => [item.interface]);
    });
  }
}
</script>
