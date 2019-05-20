<template>
  <Card title="System Properties">
    <Tabs value="name1">
      <TabPane label="General Settings" name="name1">
        <Form :model="formItem" :label-width="80">
          <FormItem label="Local Time">
            <Input v-model="formItem.input" placeholder="Enter something..." />
          </FormItem>
          <FormItem label="Hostname">
            <Input v-model="formItem.input" placeholder="Enter something..." />
          </FormItem>
          <FormItem label="Timezone">
            <Select v-model="formItem.input">
              <Option value="UTC">UTC</Option>
              <Option value="Africa/Abidjan">Africa/Abidjan</Option>
            </Select>
          </FormItem>
        </Form>
      </TabPane>
      <TabPane label="Logging" name="name2">标签二的内容</TabPane>
      <TabPane label="Language and Style" name="name3">标签三的内容</TabPane>
    </Tabs>
  </Card>
</template>

<script>

export default {
  name: 'system',
  data() {
    return {
      formItem: {
        input: ''
      }
    }
  },
  mounted() {
  }
}
</script>
