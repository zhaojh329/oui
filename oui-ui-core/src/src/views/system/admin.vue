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
        </Card>
      </TabPane>
      <TabPane label="SSH Access" name="dropbear">
      </TabPane>
      <TabPane label="SSH-Keys" name="sshkeys">
      </TabPane>
    </Tabs>
    <div style="position:absolute; right: 100px; margin-top: 10px">
      <Button type="primary" style="margin-right: 10px" @click="save">Save</Button>
      <Button type="warning" @click="reset">Reset</Button>
    </div>
  </div>
</template>

<script>

export default {
  name: 'admin',
  data() {
    return {
      tab: 'password',
      password: '',
      confirmation: ''
    }
  },
  methods: {
    save() {
      if (this.tab === 'password') {
        this.$session.get(r => {
          this.$system.setPassword(r.username, this.password).then(() => {
            this.$router.push('/login');
          });
        });
      }
    },
    reset() {

    }
  }
}
</script>
