module.exports = [
  {
    "type": "heading",
    "defaultValue": "Legend of Zelda Watch Settings"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Settings"
      },
      {
        "type": "select",
        "messageKey": "BG_COLOR",
        "defaultValue": "green",
        "label": "Background Color",
        "options": [
          {
            "label": "Green",
            "value": "green"
          },
          {
            "label": "Red",
            "value": "red"
          },
          {
            "label": "Blue",
            "value": "blue"
          },
          {
            "label": "Orange",
            "value": "orange"
          },
          {
            "label": "Purple",
            "value": "purple"
          }
        ]
      },
      {
        "type": "radiogroup",
        "messageKey": "DATE_FORMAT",
        "label": "Date Format",
        "defaultValue": "usa",
        "options": [
          {
            "label": "MM/DD/YY",
            "value": "usa"
          },
          {
            "label": "DD/MM/YY",
            "value": "global"
          }
        ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];