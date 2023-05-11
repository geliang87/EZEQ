#pragma once

#include <JuceHeader.h>

namespace Gui
{
	class SimpleEQLookAndFeel : public LookAndFeel_V4
	{
		void drawButtonBackground(Graphics& g,
			Button& button,
			const Colour& backgroundColour,
			bool shouldDrawButtonAsHighlighted,
			bool shouldDrawButtonAsDown) override
		{
			auto cornerSize = 6.0f;
			auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

			auto baseColour = juce::Colours::darkorange.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
				.withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f);

			if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
				baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

			g.setColour(juce::Colours::darkorange);

			auto flatOnLeft = button.isConnectedOnLeft();
			auto flatOnRight = button.isConnectedOnRight();
			auto flatOnTop = button.isConnectedOnTop();
			auto flatOnBottom = button.isConnectedOnBottom();

			if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
			{
				Path path;
				path.addRoundedRectangle(bounds.getX(), bounds.getY(),
					bounds.getWidth(), bounds.getHeight(),
					cornerSize, cornerSize,
					!(flatOnLeft || flatOnTop),
					!(flatOnRight || flatOnTop),
					!(flatOnLeft || flatOnBottom),
					!(flatOnRight || flatOnBottom));

				g.fillPath(path);

				g.setColour(button.findColour(ComboBox::outlineColourId));
				g.strokePath(path, PathStrokeType(1.0f));
			}
			else
			{
				g.fillRoundedRectangle(bounds, cornerSize);

				g.setColour(Colours::lightyellow);
				g.drawRoundedRectangle(bounds, cornerSize, 3.0f);
			}
		}
	};


	class PresetPanel : public Component, Button::Listener, ComboBox::Listener
	{
	public:
		PresetPanel(Service::PresetManager& pm) : presetManager(pm)
		{
            presetLNF.setColour (PopupMenu::ColourIds::backgroundColourId, juce::Colours::darkorange);
            presetLNF.setColour (PopupMenu::ColourIds::highlightedBackgroundColourId, Colours::darkorange);
            
			configureButton(saveButton, "Save");
			configureButton(deleteButton, "Delete");
			configureButton(previousPresetButton, "<");
			configureButton(nextPresetButton, ">");

			presetList.setTextWhenNothingSelected("No Preset Selected");
			presetList.setMouseCursor(MouseCursor::PointingHandCursor);
            presetList.setColour (ComboBox::ColourIds::backgroundColourId, juce::Colours::darkorange);
            presetList.setLookAndFeel (&presetLNF);
			addAndMakeVisible(presetList);
			presetList.addListener(this);

			loadPresetList();
		}

		~PresetPanel()
		{
            presetList.setLookAndFeel (nullptr);
            
            saveButton.setLookAndFeel (nullptr);
            deleteButton.setLookAndFeel (nullptr);
            previousPresetButton.setLookAndFeel (nullptr);
            nextPresetButton.setLookAndFeel (nullptr);
            
			saveButton.removeListener(this);
			deleteButton.removeListener(this);
			previousPresetButton.removeListener(this);
			nextPresetButton.removeListener(this);
			presetList.removeListener(this);
		}

		void resized() override
		{
			const auto container = getLocalBounds().reduced(4);
			auto bounds = container;

			saveButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.2f)).reduced(4));
			previousPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
			presetList.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.4f)).reduced(4));
			nextPresetButton.setBounds(bounds.removeFromLeft(container.proportionOfWidth(0.1f)).reduced(4));
			deleteButton.setBounds(bounds.reduced(4));
		}
	private:
		void buttonClicked(Button* button) override
		{
			if (button == &saveButton)
			{
				fileChooser = std::make_unique<FileChooser>(
					"Please enter the name of the preset to save",
					Service::PresetManager::defaultDirectory,
					"*." + Service::PresetManager::extension
					);
				fileChooser->launchAsync(FileBrowserComponent::saveMode, [&](const FileChooser& chooser)
					{
						const auto resultFile = chooser.getResult();
				presetManager.savePreset(resultFile.getFileNameWithoutExtension());
				loadPresetList();
					});
			}
			if (button == &previousPresetButton)
			{
				const auto index = presetManager.loadPreviousPreset();
				presetList.setSelectedItemIndex(index, dontSendNotification);
			}
			if (button == &nextPresetButton)
			{
				const auto index = presetManager.loadNextPreset();
				presetList.setSelectedItemIndex(index, dontSendNotification);
			}
			if (button == &deleteButton)
			{
				presetManager.deletePreset(presetManager.getCurrentPreset());
				loadPresetList();
			}
		}
		void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
		{
			if (comboBoxThatHasChanged == &presetList)
			{
				presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
			}
		}

		void configureButton(TextButton& button, const String& buttonText)
		{
			button.setButtonText(buttonText);
			button.setMouseCursor(MouseCursor::PointingHandCursor);
            button.setColour (TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
            button.setColour (TextButton::ColourIds::buttonColourId, juce::Colours::darkorange);
			button.setColour(TextButton::ColourIds::textColourOffId, Colours::lightyellow);
			button.setColour(TextButton::ColourIds::textColourOnId, Colours::darkorange);
            button.setColour (TextButton::ColourIds::buttonOnColourId, Colours::yellow);
			button.setLookAndFeel(&presetLNF);
			addAndMakeVisible(button);
			button.addListener(this);
		}

		void loadPresetList()
		{
			presetList.clear(dontSendNotification);
			const auto allPresets = presetManager.getAllPresets();
			const auto currentPreset = presetManager.getCurrentPreset();
			presetList.addItemList(allPresets, 1);
			presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), dontSendNotification);
			presetList.setColour(ComboBox::ColourIds::outlineColourId, Colours::lightyellow);
		}

		Service::PresetManager& presetManager;
		TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
		ComboBox presetList;
		SimpleEQLookAndFeel presetLNF;
		std::unique_ptr<FileChooser> fileChooser;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
	};
}
